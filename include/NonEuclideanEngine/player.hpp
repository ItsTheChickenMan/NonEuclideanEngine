#pragma once

#include <NonEuclideanEngine/misc.hpp>
#include <NonEuclideanEngine/gameobjects.hpp>

#include <unordered_map>
#include <map>

namespace Knee {
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
}