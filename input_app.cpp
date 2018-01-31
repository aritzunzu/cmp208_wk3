#include "input_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <input/input_manager.h>
#include <input/touch_input_manager.h>
#include <system/debug_log.h>


//Test

InputApp::InputApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	font_(NULL),
	input_manager_(NULL),
	active_touch_id_(-1)
{
}

void InputApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	input_manager_ = gef::InputManager::Create(platform_);

	my_sprite_.set_position(480.0f, 272.0f, 0.0f);
	my_sprite_.set_width(32.0f);
	my_sprite_.set_height(32.0f);
	my_sprite_.set_colour(0xffffffff);
	touch_released = false;
	frame_ammount = 0;
	

	// make sure if there is a panel to detect touch input, then activate it
	if (input_manager_ && input_manager_->touch_manager() && (input_manager_->touch_manager()->max_num_panels() > 0))
		input_manager_->touch_manager()->EnablePanel(0);

	InitFont();
}

void InputApp::CleanUp()
{
	CleanUpFont();

	delete input_manager_;
	input_manager_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;
}

bool InputApp::Update(float frame_time)
{
	
	fps_ = 1.0f / frame_time;

	if (input_manager_)
	{
		input_manager_->Update();

		ProcessTouchInput();
		
	}
	gef::Vector4 sprite_position = my_sprite_.position();
	
	if (touch_released)
	{	
		sprite_position.set_x(sprite_position.x() + (sprite_position.x() -end_position.x)/60);
		sprite_position.set_y(sprite_position.y() + (sprite_position.y() - end_position.y) / 60);
		frame_ammount += frame_time;
	}
	if (frame_ammount >= 2)	
	{
		touch_released = false;
		frame_ammount = 0;
	}

		my_sprite_.set_position(sprite_position);

	return true;
}

void InputApp::Render()
{
	sprite_renderer_->Begin();
	sprite_renderer_->DrawSprite(my_sprite_);
	DrawHUD();
	sprite_renderer_->End();
}
void InputApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void InputApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void InputApp::DrawHUD()
{
	if(font_)
	{
		// if a touch is active lets draw some text
		if (active_touch_id_ != -1)
		{
			font_->RenderText(
				sprite_renderer_,
				gef::Vector4(touch_position_.x, touch_position_.y, -0.9f),
				1.0f, 0xffffffff, gef::TJ_LEFT,
				"(%.1f, %.1f)",
				touch_position_.x, touch_position_.y);
		}

		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
}

void InputApp::ProcessTouchInput()
{
	const gef::TouchInputManager* touch_input = input_manager_->touch_manager();
	if (touch_input && (touch_input->max_num_panels() > 0))
	{
		// get the active touches for this panel
		const gef::TouchContainer& panel_touches = touch_input->touches(0);

		// go through the touches
		for (gef::ConstTouchIterator touch = panel_touches.begin(); touch != panel_touches.end(); ++touch)
		{
			// if active touch id is -1, then we are not currently processing a touch
			if (active_touch_id_ == -1)
			{
				// check for the start of a new touch
				if (touch->type == gef::TT_NEW)
				{
					active_touch_id_ = touch->id;

					// do any processing for a new touch here
					// we're just going to record the position of the touch
					touch_position_ = touch->position;					
				}
			}
			else if (active_touch_id_ == touch->id)
			{
				// we are processing touch data with a matching id to the one we are looking for
				if (touch->type == gef::TT_ACTIVE)
				{
					// update an active touch here
					// we're just going to record the position of the touch
					touch_position_ = touch->position;
					my_sprite_.set_colour(0xff0000ff);					
				}
				else if (touch->type == gef::TT_RELEASED)
				{
					// the touch we are tracking has been released
					// perform any actions that need to happen when a touch is released here
					// we're not doing anything here apart from resetting the active touch id
					active_touch_id_ = -1;

					my_sprite_.set_colour(0xffffffff);
					end_position = touch->position;
					
				}
			}
		}
	}
}


