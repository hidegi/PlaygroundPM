#include "PM/pacman.h"

namespace pm
{
	Pacman::Pacman(sp::StateManager& controller) : 
		sp::Activity(&controller)
	{
		SP_ASSERT(m_board.loadFromFile(SP_PATH("/assets/levels")), "Failed to create board");
	}
	
	Pacman::~Pacman()
	{
	}
	
	void Pacman::onStart()
	{
	}
	
	void Pacman::onUpdate(double elapsed)
	{
	}
	
	void Pacman::onLeave()
	{
	}
	
	void Pacman::onExit()
	{
	}
	
	void Pacman::onEnter()
	{
	}
	
	void Pacman::onResume()
	{
	}
	
	void Pacman::onDraw(sp::FramebufferWrapper& surface)
	{
	}
	
	void Pacman::onEnd()
	{
	}
}