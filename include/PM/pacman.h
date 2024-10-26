#ifndef PACMAN_GAME_STATE_H
#define PACMAN_GAME_STATE_H
#include "config.h"
#include "PM/board.h"
#include <SP/state/activity.h>
#include <SP/state/state_manager.h>
#include <SP/sparse/extractor.h>

namespace pm
{
	class Board;
	class Pacman : public sp::Activity
	{
		public:
			Pacman(sp::StateManager& controller);
		   ~Pacman();
			virtual void onStart() override;
            virtual void onUpdate(double elapsed) override;
            virtual void onLeave() override;
            virtual void onExit() override;
            virtual void onEnter() override;
            virtual void onResume() override;
			virtual void onDraw(sp::FramebufferWrapper& surface) override;
			virtual void onEnd() override;
		
		private:
			Board m_board;
	};
}

#endif