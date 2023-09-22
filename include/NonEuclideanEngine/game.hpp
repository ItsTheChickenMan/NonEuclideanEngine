#pragma once

#include <NonEuclideanEngine/misc.hpp>
#include <NonEuclideanEngine/shader.hpp>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include <vector>
#include <map>
#include <unordered_map>

namespace Knee {	
	// classes //
	
	// a game object that does not move and is not affected by physics, but can still interact with other game objects.
	class StaticGameObject : public virtual GeneralObject {
		public:
			StaticGameObject();
			virtual ~StaticGameObject();

			GeneralObject* asGeneralObject();
	};
	
	// like a static game object, but now you can see it!
	class RenderableStaticGameObject : public virtual StaticGameObject, public RenderableObject {
		public:
			RenderableStaticGameObject(Knee::VertexData*, Knee::Texture2D*);
			virtual ~RenderableStaticGameObject();

			Knee::RenderableObject* asRenderableObject();
			Knee::StaticGameObject* asStaticGameObject();
	};
	
	// a game object that can physically interact with a game and be affected by physics.
	// essentially a StaticGameObject, but no longer static (crazy)
	class GameObject : public virtual StaticGameObject {
		// physics
		double m_mass = 0.0;
		glm::vec3 m_velocity = glm::vec3(0);
		
		public:
			GameObject();
			GameObject(double);
			virtual ~GameObject();
			
			Knee::StaticGameObject* asStaticGameObject();

			double getMass();
			void setMass(double);
		
			glm::vec3 getVelocity();
			
			// set the velocity to some value, ignoring mass
			void setVelocity(glm::vec3);
			
			// change velocity by some value, ignoring mass
			void changeVelocity(glm::vec3);
			
			// apply an impulse force to this object
			void impulse(glm::vec3);
			
			// change the object's position based on the provided change in time
			virtual void update(double);
	};
	
	// a game object that can be rendered in the scene.
	class RenderableGameObject : public GameObject, public RenderableStaticGameObject {
		// might be some extra data in here at some point?
		
		public:
			RenderableGameObject(Knee::VertexData*, Knee::Texture2D*);

			GameObject* asGameObject();	
	};
	
	// a "visual portal" is a surface "paired" to another visual portal.  the portal renders what would be seen through it if light travelled through the pair of portals, or in other words, it "looks" into the paired portal
	// this effect is only visual, and does not interact with the player
	class VisualPortal : public RenderableStaticGameObject {
		// how many times to re-render the world when looking at our own portal
		// basically, how many portals deep we want an infinite hallway of our own portal to be
		// higher the number --> greater the performance dip
		// TODO: add checks to only recursively render if we're positive that we're looking at our own portal
		// TODO: add checks to limit the amount of objects we have to re-render
		const static uint32_t RECURSIVE_WORLD_RENDER_COUNT = 12;

		// the paired portal used to determine what the camera should see when viewing this portal.  a paired portal does not have to pair with this portal in order to work
		// a portal can also pair with itself, which is effectively the same as not existing at all (won't be rendered).  this can be useful for portals that you want to use as an output for another portal but you don't want to pair back (one way hallway sort of effect)
		Knee::VisualPortal* m_pair;

		// main rendering framebuffer + renderbuffer used for rendering the final portal texture
		uint32_t m_mainFramebuffer;
		uint32_t m_mainRenderbuffer;

		// auxiliary framebuffer + renderbuffer used for rendering the world recursively when the portal has to visualize itself
		uint32_t m_auxFramebuffer;
		uint32_t m_auxRenderbuffer;

		// secondary texture used for aux framebuffer
		Knee::Texture2D* m_mainTexture;
		
		// another secondary texture
		Knee::Texture2D* m_auxTexture;

		public:
			VisualPortal(Knee::VertexData* vertexData, uint32_t screenWidth, uint32_t screenHeight);
			~VisualPortal();

			Knee::RenderableStaticGameObject* asRenderableStaticGameObject();

			// pair another portal with this portal.  can pass self in order to create a blank portal
			void pairVisualPortal(Knee::VisualPortal* portal);

			void loadPortalTexture(std::vector<RenderableObject*>* renderableObjects, Knee::RenderableObjectShaderProgram* renderableObjectShaderProgramWithDepth);

			void draw();

			bool isOwnPair();
	};


	// a class which stores keybinds to player inputs and used as a medium between Application sdl event handling and Game input handling
	class PlayerInputHandler {
		// keybinds
		// defaults are WASD with Space mapped to up and left shift mapped to down
		std::unordered_map<uint32_t, SDL_Scancode> m_keybinds = {
			{ KEYBIND_FORWARD, SDL_SCANCODE_W },
			{ KEYBIND_BACKWARD, SDL_SCANCODE_S },
			{ KEYBIND_STRAFELEFT, SDL_SCANCODE_A },
			{ KEYBIND_STRAFERIGHT, SDL_SCANCODE_D },
			{ KEYBIND_UP, SDL_SCANCODE_SPACE },
			{ KEYBIND_DOWN, SDL_SCANCODE_LSHIFT },
			{ KEYBIND_SPRINT, SDL_SCANCODE_Q },
			{ KEYBIND_UNLOCK_CURSOR, SDL_SCANCODE_ESCAPE }
		};
		
		// all keystates
		std::unordered_map<SDL_Scancode, bool> m_keystates;
		
		// mouse states
		// key = one of the SDL mouse button macros, value = true if pressed or false if not
		std::map<uint8_t, bool> m_mouseButtonStates;
		
		// motion of the mouse
		// should be reset to (0, 0) prior to polling events
		glm::vec2 m_mouseMotion = glm::vec2(0);
		
		public:
			static const uint32_t KEYBIND_FORWARD;
			static const uint32_t KEYBIND_BACKWARD;
			static const uint32_t KEYBIND_STRAFELEFT;
			static const uint32_t KEYBIND_STRAFERIGHT;
			static const uint32_t KEYBIND_UP;
			static const uint32_t KEYBIND_DOWN;
			static const uint32_t KEYBIND_SPRINT;
			static const uint32_t KEYBIND_UNLOCK_CURSOR;
			
			PlayerInputHandler();
			
			// KEYBOARD METHODS //
			
			bool keybindExists(uint32_t);
			
			// get the scancode currently bound to the named bind
			SDL_Scancode getKeybind(uint32_t);
			
			// bind a scancode to a named bind
			void keybind(uint32_t, SDL_Scancode);
			
			// gets the current state of any scancode.
			bool getKeyState(SDL_Scancode);
			
			// gets the current state of one of the named binds.  this is generally preferable as it allows binds to be changed easily through this class, rather than hardcoding a particular key
			bool getKeybindState(uint32_t);
			
			// MOUSE BUTTON METHODS //
			
			// get the current press state for a provided mouse button (use SDL macros)
			bool getMouseButtonState(uint8_t);
			
			// MOUSE MOTION METHODS //
			
			glm::vec2 getMouseMotion();
			
			// reset mouse motion to (0, 0)
			// call this before polling events
			void resetMouseMotion();
			
			// EVENT HANDLING //
			
			// call before polling events
			void reset();
			
			void processKeyboardEvent(SDL_KeyboardEvent&);
			void processMouseButtonEvent(SDL_MouseButtonEvent&);
			void processMouseMotionEvent(SDL_MouseMotionEvent&);
			
			void processEvent(SDL_Event&);
	};
	
	// a game object that the renderer uses as a camera.  also contains additional data related to player interaction
	class Player : public GameObject {
		// x rotation limit
		static const double X_ROTATION_LIMIT;
		
		// motion vectors per bind
		static const std::map<uint32_t, glm::vec3> KEYBIND_MOTION_VECTORS;
		
		// members //
		
		// settings
		double m_mouseSensitivity = 1.0; // multiplied by 1/500, then multiplied by mouse delta in pixels
		
		double m_playerSpeed = 2.0; // general speed factor (units tbd)
		double m_sprintMultiplier = 3.0; // how much speed is multiplied by when sprint bind is pressed
		
		// input handler
		Knee::PlayerInputHandler m_inputHandler;
		
		public:
			Player();
			Player(double);
			
			Knee::PlayerInputHandler* getInputHandler();
			
			// returns true if mouse is locked, false otherwise.
			// if the mouse isn't locked currently, it locks the mouse if the mouse has just been pressed
			// if the mouse is locked currently, it unlocks it if KEYBIND_UNLOCK_CURSOR is pressed
			bool checkMouseLock();
			
			// restricts the player's x rotation to between -X_ROTATION_LIMIT and X_ROTATION_LIMIT (called automatically during update())
			void lockXRotation();
			
			// handles all player movement from mouse movement
			void handleMouseMovement();
			
			// handles all player movement from keyboard presses
			void handleKeyboard(double);
			
			// determines the player's desired "motion" vector based on their keypresses.  note that this is not guaranteed to translate into actual player motion
			glm::vec3 getDesiredMotionVector();
			
			// updates movement from inputs, etc.
			// automatically called during game updates
			void update(double);
	};
	
	class Game {
		// shader paths //
		static const std::string SHADER_ROOT;
		static const std::string RENDERABLE_GAMEOBJECT_SHADER_ROOT;
		static const std::string RENDERABLE_GAMEOBJECT_VERTEX_SHADER_PATH;
		static const std::string RENDERABLE_GAMEOBJECT_FRAGMENT_SHADER_PATH;
		static const std::string VISUAL_PORTAL_SHADER_ROOT;
		static const std::string VISUAL_PORTAL_VERTEX_SHADER_PATH;
		static const std::string VISUAL_PORTAL_FRAGMENT_SHADER_PATH;
		static const std::string RENDERABLE_GAMEOBJECT_WITH_DEPTH_SHADER_ROOT;
		static const std::string RENDERABLE_GAMEOBJECT_WITH_DEPTH_VERTEX_SHADER_PATH;
		static const std::string RENDERABLE_GAMEOBJECT_WITH_DEPTH_FRAGMENT_SHADER_PATH;
		
		// the player
		Knee::Player m_player;
		
		// map of all static game objects, mapped by id
		std::map<std::string, StaticGameObject*> m_staticGameObjects;

		// map of all game objects, mapped by id
		std::map<std::string, GameObject*> m_gameObjects;
		
		// list of all renderable objects, added to whenever a type of renderable game object is added
		std::vector<RenderableObject*> m_renderableGameObjects;
		
		// map of all visual portals, mapped by id
		std::vector<VisualPortal*> m_visualPortals;

		// shaders
		Knee::RenderableObjectShaderProgram m_renderableGameObjectShaderProgram;
		Knee::RenderableObjectShaderProgram m_visualPortalShaderProgram;
		Knee::RenderableObjectShaderProgram m_renderableGameObjectWithDepthShaderProgram;

		public:
			Game(uint32_t, uint32_t);
			~Game();
		
			Knee::Player* getPlayer();
			
			void initialize();
			
			Knee::StaticGameObject* getStaticGameObject(std::string);
			Knee::GameObject* getGameObject(std::string);
			
			void addStaticGameObject(std::string id, StaticGameObject* obj);
			void addGameObject(std::string id, GameObject* obj);

			void addRenderableStaticGameObject(std::string id, RenderableStaticGameObject* obj);
			void addRenderableGameObject(std::string id, RenderableGameObject* obj);
			
			void addVisualPortal(std::string id, VisualPortal* portal);

			void updateGameObjects(double);
			void updatePlayer(double);
			
			// renders both static and non-static game objects
			void renderAllRenderableGameObjects();
			void updateVisualPortals();

			void renderScene();
			
			Knee::PerspectiveCamera* getPlayerCamera();
			void updateCamera();
			
			void processEvent(SDL_Event&);
	};
}