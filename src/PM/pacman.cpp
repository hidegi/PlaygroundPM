#include "PM/pacman.h"

namespace pm
{
	Pacman::Pacman(sp::StateManager& controller) : 
		sp::Activity(&controller)
	{
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
		sp::gldraw::setGLStates();
		SP_ASSERT(m_board.loadFromFile(SP_PATH("/assets/levels")), "Failed to create board");
	}
	
	void Pacman::onResume()
	{
		sp::gldraw::resetGLStates();
	}
	
	
	void Pacman::onDraw(sp::FramebufferWrapper& surface)
	{
		surface.activate();
		surface.clear(sp::Color(0,0,0,255));
		
		const sp::Texture* tex = m_board.getLevelTexture();
		SP_ASSERT(tex, "");
		sp::Texture::bind(tex);
		sp::gldraw::drawFrame(0, 0, m_board.getSize().x * 16, m_board.getSize().y * 16);
	}
	
	void Pacman::onEnd()
	{
	}
}