/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Feb, 11th 2021
 * Version 0.0.1 (Alpha)
 * events.cpp
 */

#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <map>

#include <SDL2/SDL.h>

#include "chartdisplay.hpp"
#include "utils.hpp"
#include "events.hpp"


struct find_id : std::unary_function<Btn_Callback, bool>
{
  uint id;
  find_id(uint id):id(id) { }
  bool operator()(Btn_Callback const& btn) const
  {
    return btn.id == id;
  }
};

int add_btn(uint id, SDL_Rect rect, void (*ptr)())
{
  std::vector<Btn_Callback>::iterator it = std::find_if(
    btn_callback_list.begin(),
    btn_callback_list.end(),
    find_id(id)
  );

  if (it != btn_callback_list.end())
  {
    std::cerr << "Button callback addition for " << ptr << " failed. ID " << id << " already in use." << std::endl;
    return -1;
  }
  else
  {
    Btn_Callback new_btn = {id, rect, ptr};
    btn_callback_list.push_back(new_btn);
    return 0;
  }
}

int remove_btn(uint id)
{
  std::vector<Btn_Callback>::iterator it = std::find_if(
    btn_callback_list.begin(),
    btn_callback_list.end(),
    find_id(id)
  );

  if (it != btn_callback_list.end())
  {
    btn_callback_list.erase(it);
    return 0;
  }
  else
  {
    std::cerr << "Button callback removal failed. ID " << id << " does not exist." << std::endl;
    return -1;
  }
}

void replace_btn(uint id, SDL_Rect rect, void (*ptr)())
{
  remove_btn(id);
  add_btn(id, rect, ptr);
}

void remove_all_btns(void)
{
  if (btn_callback_list.size() > 0)
  {
    btn_callback_list.clear();
  }
}

int clicks_test = 0;
void print_click(void)
{
  std::cout << "C L I C K" << clicks_test++ << std::endl;
}

void spawn_window(void)
{
  SDL_Window *window;

  //SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow(
      "NEW WINDOW",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      640,
      480,
      SDL_WINDOW_OPENGL
  );

}





bool left_mouse_pressed = false;
int mouse_x_old = 0;
int mouse_y_old = 0;
int mouse_x = 0;
int mouse_y = 0;
short mouse_h = 0;

int mouse_down_x = -1;
int mouse_down_y = -1;
int mouse_up_x = -1;
int mouse_up_y = -1;

void mpe::Events::check_mouse_hover(void)
{
  if (Active_Screen == GRAPH)
  {
    const Chart_Data chart = Active_Chart;

    if (chart.rows == 0) return;

  	const ushort bar_width = total_bar_width;

  	const int scale = s_pan;
  	const int offset = window_h + v_pan;

  	uint adj_h_pan = (mouse_x / bar_width) + h_pan;

    if (adj_h_pan >= chart.rows) return;

  	//uint y_pos_top = -(chart.hv.at(adj_h_pan) * scale) + offset;
  	//uint y_pos_bottom = -(chart.lv.at(adj_h_pan) * scale) + offset;

  	double y_chart_top = -(chart.hv.at(adj_h_pan) * scale) + offset;
  	double y_chart_bottom = -(chart.lv.at(adj_h_pan) * scale) + offset;

  	//stops floating point exeption & makes hitbox large enough for the mouse to hover over
  	if (y_chart_bottom - y_chart_top < 2.0d)
  	{
  		y_chart_bottom += 1.0;
  		y_chart_top -= 1.0;
  	}

  	//if bar is out of bounds, push collision bounds into positive range
  	if (y_chart_top <= 0.0d) y_chart_top = 0.0;
  	if (y_chart_bottom <= 0.0d) y_chart_bottom = 1.0;

  	uint y_pos_top = static_cast<uint>(y_chart_top);
  	uint y_pos_bottom = static_cast<uint>(y_chart_bottom);

  	uint bar_height = y_pos_bottom - y_pos_top;

  	if (static_cast<uint>(mouse_y) > y_pos_top && static_cast<uint>(mouse_y) < y_pos_bottom)
  	{
  		//used to pad the round down, so that it will only check for collision inside of the bar
  		int y_padding = (y_pos_top % bar_height);

  		int adj_hov_x = hovering_mouse_x / bar_width;
  		int adj_hov_y = (hovering_mouse_y - y_padding) / bar_height;
  		int adj_mou_x = mouse_x / bar_width;
  		int adj_mou_y = (mouse_y - y_padding) / bar_height;
  		if (
  				(adj_hov_x != adj_mou_x) ||
  				(adj_hov_y != adj_mou_y)
  			)
  		{
  			hovering_mouse_x = mouse_x;
  			hovering_mouse_y = mouse_y;
  			mouse_hovering = true;
  			repaint_hovering = true;
  			repaint = true;
  		}
  	}
  	else
  	{
  		if (mouse_hovering)
  		{
  			mouse_hovering = false;
  			repaint = true;

  			// + bar_height pushes collision outside of bar so
  			//that the next time the cursor collides it will redraw
  			hovering_mouse_x = mouse_x;
  			hovering_mouse_y = mouse_y + bar_height;
  		}
  	}
  }
}


void mpe::Events::check_mouse_click(void)
{
  for (uint i = 0; i < btn_callback_list.size(); i++)
  {
    SDL_Rect btn_rect = btn_callback_list.at(i).rect;

    //if mouse within bounds
    if (
      (mouse_up_x >= btn_rect.x) &&
      (mouse_up_x <= (btn_rect.x + btn_rect.w)) &&
      (mouse_up_y >= btn_rect.y) &&
      (mouse_up_y <= (btn_rect.y + btn_rect.h))
    ){
      if (
        (mouse_down_x >= btn_rect.x) &&
        (mouse_down_x <= (btn_rect.x + btn_rect.w)) &&
        (mouse_down_y >= btn_rect.y) &&
        (mouse_down_y <= (btn_rect.y + btn_rect.h))
      ){
        (btn_callback_list.at(i).ptr)();//call callback function
      }
      //reset remembered loc so that button doesn't keep being called
      mouse_down_x = -1;
      mouse_down_y = -1;
      mouse_up_x = -1;
      mouse_up_y = -1;
      //break; disabled because it's posible that two buttons overlap
    }
  }
}

enum Input_Type : uint
{
  /* Keydown events */
	FIND_DOWN, UNDO_DOWN, COPY_DOWN, CUT_DOWN, PASTE_DOWN,
	NUMLOCKCLEAR_DOWN, KP_DIVIDE_DOWN, KP_MULTIPLY_DOWN, KP_MINUS_DOWN, KP_PLUS_DOWN, KP_ENTER_DOWN, KP_PERIOD_DOWN,
	KP_1_DOWN, KP_2_DOWN, KP_3_DOWN, KP_4_DOWN, KP_5_DOWN, KP_6_DOWN, KP_7_DOWN, KP_8_DOWN, KP_9_DOWN, KP_0_DOWN,
	LALT_DOWN, RALT_DOWN, LCTRL_DOWN, RCTRL_DOWN, LMETA_DOWN, RMETA_DOWN, LSHIFT_DOWN, RSHIFT_DOWN,
	UP_DOWN, LEFT_DOWN, RIGHT_DOWN, DOWN_DOWN, ESCAPE_DOWN,
	F1_DOWN, F2_DOWN, F3_DOWN, F4_DOWN, F5_DOWN, F6_DOWN, F7_DOWN, F8_DOWN, F9_DOWN, F10_DOWN, F11_DOWN, F12_DOWN,
	TAB_DOWN, CAPSLOCK_DOWN, BACKQUOTE_DOWN,
	K_1_DOWN, K_2_DOWN, K_3_DOWN, K_4_DOWN, K_5_DOWN, K_6_DOWN, K_7_DOWN, K_8_DOWN, K_9_DOWN, K_0_DOWN,
	EXCLAIM_DOWN, AT_DOWN, HASH_DOWN, DOLLAR_DOWN, PERCENT_DOWN, CARET_DOWN, AMPERSAND_DOWN, ASTERISK_DOWN,
	LEFTPAREN_DOWN, RIGHTPAREN_DOWN, MINUS_DOWN, UNDERSCORE_DOWN, PLUS_DOWN, EQUALS_DOWN, BACKSPACE_DOWN,
	LEFTBRACKET_DOWN, KP_LEFTBRACE_DOWN, RIGHTBRACKET_DOWN, KP_RIGHTBRACE_DOWN,
	BACKSLASH_DOWN, KP_VERTICALBAR_DOWN, SEMICOLON_DOWN, COLON_DOWN, QUOTE_DOWN, QUOTEDBL_DOWN,
	RETURN_DOWN, COMMA_DOWN, LESS_DOWN, PERIOD_DOWN, GREATER_DOWN, SLASH_DOWN, QUESTION_DOWN, SPACE_DOWN,
	PRINTSCREEN_DOWN, SCROLLLOCK_DOWN, PAUSE_DOWN, INSERT_DOWN, HOME_DOWN, PAGEUP_DOWN, DELETE_DOWN, END_DOWN, PAGEDOWN_DOWN,
	A_DOWN, B_DOWN, C_DOWN, D_DOWN, E_DOWN, F_DOWN, G_DOWN, H_DOWN, I_DOWN, J_DOWN, K_DOWN, L_DOWN, M_DOWN,
	N_DOWN, O_DOWN, P_DOWN, Q_DOWN, R_DOWN, S_DOWN, T_DOWN, U_DOWN, V_DOWN, W_DOWN, X_DOWN, Y_DOWN, Z_DOWN,

  /* Keyup events*/
	FIND_UP, UNDO_UP, COPY_UP, CUT_UP, PASTE_UP,
	NUMLOCKCLEAR_UP, KP_DIVIDE_UP, KP_MULTIPLY_UP, KP_MINUS_UP, KP_PLUS_UP, KP_ENTER_UP, KP_PERIOD_UP,
	KP_1_UP, KP_2_UP, KP_3_UP, KP_4_UP, KP_5_UP, KP_6_UP, KP_7_UP, KP_8_UP, KP_9_UP, KP_0_UP,
	LALT_UP, RALT_UP, LCTRL_UP, RCTRL_UP, LMETA_UP, RMETA_UP, LSHIFT_UP, RSHIFT_UP,
	UP_UP, LEFT_UP, RIGHT_UP, DOWN_UP, ESCAPE_UP,
	F1_UP, F2_UP, F3_UP, F4_UP, F5_UP, F6_UP, F7_UP, F8_UP, F9_UP, F10_UP, F11_UP, F12_UP,
	TAB_UP, CAPSLOCK_UP, BACKQUOTE_UP,
	K_1_UP, K_2_UP, K_3_UP, K_4_UP, K_5_UP, K_6_UP, K_7_UP, K_8_UP, K_9_UP, K_0_UP,
	EXCLAIM_UP, AT_UP, HASH_UP, DOLLAR_UP, PERCENT_UP, CARET_UP, AMPERSAND_UP, ASTERISK_UP,
	LEFTPAREN_UP, RIGHTPAREN_UP, MINUS_UP, UNDERSCORE_UP, PLUS_UP, EQUALS_UP, BACKSPACE_UP,
	LEFTBRACKET_UP, KP_LEFTBRACE_UP, RIGHTBRACKET_UP, KP_RIGHTBRACE_UP,
	BACKSLASH_UP, KP_VERTICALBAR_UP, SEMICOLON_UP, COLON_UP, QUOTE_UP, QUOTEDBL_UP,
	RETURN_UP, COMMA_UP, LESS_UP, PERIOD_UP, GREATER_UP, SLASH_UP, QUESTION_UP, SPACE_UP,
	PRINTSCREEN_UP, SCROLLLOCK_UP, PAUSE_UP, INSERT_UP, HOME_UP, PAGEUP_UP, DELETE_UP, END_UP, PAGEDOWN_UP,
	A_UP, B_UP, C_UP, D_UP, E_UP, F_UP, G_UP, H_UP, I_UP, J_UP, K_UP, L_UP, M_UP,
	N_UP, O_UP, P_UP, Q_UP, R_UP, S_UP, T_UP, U_UP, V_UP, W_UP, X_UP, Y_UP, Z_UP,

  /* Mouse events */
	MOUSE_SCROLL_UP, MOUSE_SCROLL_DOWN,
	MOUSE_LEFT_DOWN, MOUSE_LEFT_UP,
	MOUSE_RIGHT_DOWN, MOUSE_RIGHT_UP,
	MOUSE_MOTION,

  /* For getting size of list */
	END_OF_LIST
};
std::map<Input_Type, bool> event_map;

void mpe::Events::handle_sdl_event(SDL_Event &event)
{
	switch (event.type)
	{
  	case SDL_QUIT:
  		running = false;
  		break;
    case SDL_WINDOWEVENT:
  		if (event.window.event == SDL_WINDOWEVENT_RESIZED)
  		{
  			repaint = true;
  		}
  		break;
  	case SDL_KEYDOWN:
      switch (event.key.keysym.sym)
      {
        case SDLK_FIND:
          event_map[FIND_DOWN] = true;
          break;
        case SDLK_UNDO:
          event_map[UNDO_DOWN] = true;
          break;
        case SDLK_COPY:
          event_map[COPY_DOWN] = true;
          break;
        case SDLK_CUT:
          event_map[CUT_DOWN] = true;
          break;
        case SDLK_PASTE:
          event_map[PASTE_DOWN] = true;
          break;
        case SDLK_NUMLOCKCLEAR:
          event_map[NUMLOCKCLEAR_DOWN] = true;
          break;
        case SDLK_KP_DIVIDE:
          event_map[KP_DIVIDE_DOWN] = true;
          break;
        case SDLK_KP_MULTIPLY:
          event_map[KP_MULTIPLY_DOWN] = true;
          break;
        case SDLK_KP_MINUS:
          event_map[KP_MINUS_DOWN] = true;
          break;
        case SDLK_KP_PLUS:
          event_map[KP_PLUS_DOWN] = true;
          break;
        case SDLK_KP_ENTER:
          event_map[KP_ENTER_DOWN] = true;
          break;
        case SDLK_KP_PERIOD:
          event_map[KP_PERIOD_DOWN] = true;
          break;
        case SDLK_KP_1:
          event_map[KP_1_DOWN] = true;
          break;
        case SDLK_KP_2:
          event_map[KP_2_DOWN] = true;
          break;
        case SDLK_KP_3:
          event_map[KP_3_DOWN] = true;
          break;
        case SDLK_KP_4:
          event_map[KP_4_DOWN] = true;
          break;
        case SDLK_KP_5:
          event_map[KP_5_DOWN] = true;
          break;
        case SDLK_KP_6:
          event_map[KP_6_DOWN] = true;
          break;
        case SDLK_KP_7:
          event_map[KP_7_DOWN] = true;
          break;
        case SDLK_KP_8:
          event_map[KP_8_DOWN] = true;
          break;
        case SDLK_KP_9:
          event_map[KP_9_DOWN] = true;
          break;
        case SDLK_KP_0:
          event_map[KP_0_DOWN] = true;
          break;
        case SDLK_LALT:
          event_map[LALT_DOWN] = true;
          break;
        case SDLK_RALT:
          event_map[RALT_DOWN] = true;
          break;
        case SDLK_LCTRL:
          event_map[LCTRL_DOWN] = true;
          break;
        case SDLK_RCTRL:
          event_map[RCTRL_DOWN] = true;
          break;
        case SDLK_LGUI:
          event_map[LMETA_DOWN] = true;
          break;
        case SDLK_RGUI:
          event_map[RMETA_DOWN] = true;
          break;
        case SDLK_LSHIFT:
          event_map[LSHIFT_DOWN] = true;
          break;
        case SDLK_RSHIFT:
          event_map[RSHIFT_DOWN] = true;
          break;
        case SDLK_UP:
          event_map[UP_DOWN] = true;
          break;
        case SDLK_LEFT:
          event_map[LEFT_DOWN] = true;
          break;
        case SDLK_RIGHT:
          event_map[RIGHT_DOWN] = true;
          break;
        case SDLK_DOWN:
          event_map[DOWN_DOWN] = true;
          break;
        case SDLK_ESCAPE:
          event_map[ESCAPE_DOWN] = true;
          break;
        case SDLK_F1:
          event_map[F1_DOWN] = true;
          break;
        case SDLK_F2:
          event_map[F2_DOWN] = true;
          break;
        case SDLK_F3:
          event_map[F3_DOWN] = true;
          break;
        case SDLK_F4:
          event_map[F4_DOWN] = true;
          break;
        case SDLK_F5:
          event_map[F5_DOWN] = true;
          break;
        case SDLK_F6:
          event_map[F6_DOWN] = true;
          break;
        case SDLK_F7:
          event_map[F7_DOWN] = true;
          break;
        case SDLK_F8:
          event_map[F8_DOWN] = true;
          break;
        case SDLK_F9:
          event_map[F9_DOWN] = true;
          break;
        case SDLK_F10:
          event_map[F10_DOWN] = true;
          break;
        case SDLK_F11:
          event_map[F11_DOWN] = true;
          break;
        case SDLK_F12:
          event_map[F12_DOWN] = true;
          break;
        case SDLK_TAB:
          event_map[TAB_DOWN] = true;
          break;
        case SDLK_CAPSLOCK:
          event_map[CAPSLOCK_DOWN] = true;
          break;
        case SDLK_BACKQUOTE:
          event_map[BACKQUOTE_DOWN] = true;
          break;
        case SDLK_1:
          event_map[K_1_DOWN] = true;
          break;
        case SDLK_2:
          event_map[K_2_DOWN] = true;
          break;
        case SDLK_3:
          event_map[K_3_DOWN] = true;
          break;
        case SDLK_4:
          event_map[K_4_DOWN] = true;
          break;
        case SDLK_5:
          event_map[K_5_DOWN] = true;
          break;
        case SDLK_6:
          event_map[K_6_DOWN] = true;
          break;
        case SDLK_7:
          event_map[K_7_DOWN] = true;
          break;
        case SDLK_8:
          event_map[K_8_DOWN] = true;
          break;
        case SDLK_9:
          event_map[K_9_DOWN] = true;
          break;
        case SDLK_0:
          event_map[K_0_DOWN] = true;
          break;
        case SDLK_EXCLAIM:
          event_map[EXCLAIM_DOWN] = true;
          break;
        case SDLK_AT:
          event_map[AT_DOWN] = true;
          break;
        case SDLK_HASH:
          event_map[HASH_DOWN] = true;
          break;
        case SDLK_DOLLAR:
          event_map[DOLLAR_DOWN] = true;
          break;
        case SDLK_PERCENT:
          event_map[PERCENT_DOWN] = true;
          break;
        case SDLK_CARET:
          event_map[CARET_DOWN] = true;
          break;
        case SDLK_AMPERSAND:
          event_map[AMPERSAND_DOWN] = true;
          break;
        case SDLK_ASTERISK:
          event_map[ASTERISK_DOWN] = true;
          break;
        case SDLK_LEFTPAREN:
          event_map[LEFTPAREN_DOWN] = true;
          break;
        case SDLK_RIGHTPAREN:
          event_map[RIGHTPAREN_DOWN] = true;
          break;
        case SDLK_MINUS:
          event_map[MINUS_DOWN] = true;
          break;
        case SDLK_UNDERSCORE:
          event_map[UNDERSCORE_DOWN] = true;
          break;
        case SDLK_PLUS:
          event_map[PLUS_DOWN] = true;
          break;
        case SDLK_EQUALS:
          event_map[EQUALS_DOWN] = true;
          break;
        case SDLK_BACKSPACE:
          event_map[BACKSPACE_DOWN] = true;
          break;
        case SDLK_LEFTBRACKET:
          event_map[LEFTBRACKET_DOWN] = true;
          break;
        case SDLK_KP_LEFTBRACE:
          event_map[KP_LEFTBRACE_DOWN] = true;
          break;
        case SDLK_RIGHTBRACKET:
          event_map[RIGHTBRACKET_DOWN] = true;
          break;
        case SDLK_KP_RIGHTBRACE:
          event_map[KP_RIGHTBRACE_DOWN] = true;
          break;
        case SDLK_BACKSLASH:
          event_map[BACKSLASH_DOWN] = true;
          break;
        case SDLK_KP_VERTICALBAR:
          event_map[KP_VERTICALBAR_DOWN] = true;
          break;
        case SDLK_SEMICOLON:
          event_map[SEMICOLON_DOWN] = true;
          break;
        case SDLK_COLON:
          event_map[COLON_DOWN] = true;
          break;
        case SDLK_QUOTE:
          event_map[QUOTE_DOWN] = true;
          break;
        case SDLK_QUOTEDBL:
          event_map[QUOTEDBL_DOWN] = true;
          break;
        case SDLK_RETURN:
        case SDLK_RETURN2:
          event_map[RETURN_DOWN] = true;
          break;
        case SDLK_COMMA:
          event_map[COMMA_DOWN] = true;
          break;
        case SDLK_LESS:
          event_map[LESS_DOWN] = true;
          break;
        case SDLK_PERIOD:
          event_map[PERIOD_DOWN] = true;
          break;
        case SDLK_GREATER:
          event_map[GREATER_DOWN] = true;
          break;
        case SDLK_SLASH:
          event_map[SLASH_DOWN] = true;
          break;
        case SDLK_QUESTION:
          event_map[QUESTION_DOWN] = true;
          break;
        case SDLK_SPACE:
          event_map[SPACE_DOWN] = true;
          break;
        case SDLK_PRINTSCREEN:
          event_map[PRINTSCREEN_DOWN] = true;
          break;
        case SDLK_SCROLLLOCK:
          event_map[SCROLLLOCK_DOWN] = true;
          break;
        case SDLK_PAUSE:
          event_map[PAUSE_DOWN] = true;
          break;
        case SDLK_INSERT:
          event_map[INSERT_DOWN] = true;
          break;
        case SDLK_HOME:
          event_map[HOME_DOWN] = true;
          break;
        case SDLK_PAGEUP:
          event_map[PAGEDOWN_UP] = true;
          break;
        case SDLK_DELETE:
          event_map[DELETE_DOWN] = true;
          break;
        case SDLK_END:
          event_map[END_DOWN] = true;
          break;
        case SDLK_PAGEDOWN:
          event_map[PAGEDOWN_DOWN] = true;
          break;
        case SDLK_a:
          event_map[A_DOWN] = true;
          break;
        case SDLK_b:
          event_map[B_DOWN] = true;
          break;
        case SDLK_c:
          event_map[C_DOWN] = true;
          break;
        case SDLK_d:
          event_map[D_DOWN] = true;
          break;
        case SDLK_e:
          event_map[E_DOWN] = true;
          break;
        case SDLK_f:
          event_map[F_DOWN] = true;
          break;
        case SDLK_g:
          event_map[G_DOWN] = true;
          break;
        case SDLK_h:
          event_map[H_DOWN] = true;
          break;
        case SDLK_i:
          event_map[I_DOWN] = true;
          break;
        case SDLK_j:
          event_map[J_DOWN] = true;
          break;
        case SDLK_k:
          event_map[K_DOWN] = true;
          break;
        case SDLK_l:
          event_map[L_DOWN] = true;
          break;
        case SDLK_m:
          event_map[M_DOWN] = true;
          break;
        case SDLK_n:
          event_map[N_DOWN] = true;
          break;
        case SDLK_o:
          event_map[O_DOWN] = true;
          break;
        case SDLK_p:
          event_map[P_DOWN] = true;
          break;
        case SDLK_q:
          event_map[Q_DOWN] = true;
          break;
        case SDLK_r:
          event_map[R_DOWN] = true;
          break;
        case SDLK_s:
          event_map[S_DOWN] = true;
          break;
        case SDLK_t:
          event_map[T_DOWN] = true;
          break;
        case SDLK_u:
          event_map[U_DOWN] = true;
          break;
        case SDLK_v:
          event_map[V_DOWN] = true;
          break;
        case SDLK_w:
          event_map[W_DOWN] = true;
          break;
        case SDLK_x:
          event_map[X_DOWN] = true;
          break;
        case SDLK_y:
          event_map[Y_DOWN] = true;
          break;
        case SDLK_z:
          event_map[Z_DOWN] = true;
          break;
      }
      break;

		case SDL_MOUSEWHEEL:
			if (event.wheel.y > 0)//scroll up
			{
        event_map[MOUSE_SCROLL_UP] = true;
			}
			else if (event.wheel.y < 0)//scroll down
			{
        event_map[MOUSE_SCROLL_DOWN] = true;
			}
			if (event.wheel.x > 0)//scroll right
			{

      }
			else if (event.wheel.x < 0)//scroll left
			{

      }
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT)
			{
        event_map[MOUSE_LEFT_DOWN] = true;
			}
      if (event.button.button == SDL_BUTTON_RIGHT)
			{
        event_map[MOUSE_RIGHT_DOWN] = true;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT)
			{
        event_map[MOUSE_LEFT_UP] = true;
			}
      if (event.button.button == SDL_BUTTON_RIGHT)
			{
        event_map[MOUSE_RIGHT_UP] = true;
			}
			break;

		case SDL_MOUSEMOTION:
      event_map[MOUSE_MOTION] = true;
      mouse_x = event.motion.x;
      mouse_y = event.motion.y;
			break;
	}



  /*
   *fix in the future
   *idk how to get the c&d to not 'snap' as you
   *speed twards the beginning of the chart
   *placing the code block in the events loop fixes it
   *maybe it needs to run many more times per event cycle
   *then the rest of the event handling code
   *still have no clue why though
   */
  if (Active_Screen == GRAPH)
  {
  	//click and drag scrolling
  	if (left_mouse_pressed)
  	{
  		//vertical movement
  		v_pan += mouse_y - mouse_y_old;

  		//horizontal movement
  		int mouse_dif = -(mouse_x - mouse_x_old);
  		if (mouse_dif < 0)
  		{
  			if (h_pan < static_cast<uint>(-mouse_dif)) h_pan = 0;
  			else mouse_h += mouse_dif;
  		}
  		if (mouse_dif > 0)
  		{
  			if (h_pan > 0xffffffff - mouse_dif) h_pan = 0xffffffff;
  			else mouse_h += mouse_dif;
  		}

  		ushort div = total_bar_width;
  		if (std::abs(mouse_h) > div)
  		{
  			h_pan += mouse_h / div;
  			mouse_h = mouse_h % div;
  		}

  		mouse_x_old = mouse_x;
  		mouse_y_old = mouse_y;


  		repaint = true;
  	}
  }
}




void pe_global(void)
{
  if (event_map[F1_DOWN])
  {
    reload_config();
  }
  if (event_map[F4_DOWN])
  {
    if (Active_Screen != LOG)
    {
      Active_Screen = LOG;
      repaint = true;
      remove_all_btns();
    }
  }
  if (event_map[F5_DOWN])
  {
    if (Active_Screen != GRAPH)
    {
      Active_Screen = GRAPH;
      repaint = true;
      remove_all_btns();
    }
  }
  if (event_map[F6_DOWN])
  {
    if (Active_Screen != NORMALIZED_GRAPH)
    {
      Active_Screen = NORMALIZED_GRAPH;
      repaint = true;
      remove_all_btns();
    }
  }
  if (event_map[F7_DOWN])
  {
    if (Active_Screen != SYMBOLS)
    {
      Active_Screen = SYMBOLS;
      repaint = true;
      remove_all_btns();

      add_btn(0, {0,0,0,0}, spawn_window);
    }
  }
  if (event_map[F8_DOWN])
  {
    if (Active_Screen != TRADES)
    {
      Active_Screen = TRADES;
      repaint = true;
      remove_all_btns();
    }
  }
  if (event_map[F9_DOWN])
  {
    if (Active_Screen != LOGIC)
    {
      Active_Screen = LOGIC;
      repaint = true;
      remove_all_btns();
    }
  }
  if (event_map[F10_DOWN])
  {
    if (Active_Screen != SETTINGS)
    {
      Active_Screen = SETTINGS;
      repaint = true;
      remove_all_btns();
    }
  }

  if (event_map[MOUSE_LEFT_DOWN])
  {
    left_mouse_pressed = true;

    mouse_down_x = mouse_x;
    mouse_down_y = mouse_y;
  }
  if (event_map[MOUSE_LEFT_UP])
  {
    left_mouse_pressed = false;

    mouse_up_x = mouse_x;
    mouse_up_y = mouse_y;
  }
}


void pe_log(void)
{

}

void pe_graph(void)
{
  if (event_map[F3_DOWN])
  {
    draw_moving_avg = !draw_moving_avg;
    repaint = true;
  }

  uint old_h_pan;
  int old_s_pan;
  ushort old_tbw;
  if (event_map[LEFT_DOWN])
  {
    old_h_pan = h_pan;

    if (h_pan <= 4) h_pan = 0;
    else h_pan -= 4;

    if (old_h_pan != h_pan) repaint = true;
  }
  if (event_map[RIGHT_DOWN])
  {
    old_h_pan = h_pan;

    if (h_pan >= 0xffffffff - 0x4) h_pan = 0xffffffff;
    else h_pan += 4;

    if (old_h_pan != h_pan) repaint = true;
  }
  if (event_map[UP_DOWN])
  {
    v_pan += 35;
    repaint = true;
  }
  if (event_map[DOWN_DOWN])
  {
    v_pan -= 35;
    repaint = true;
  }

  if (event_map[KP_8_DOWN])
  {
    old_s_pan = s_pan;

    if (s_pan >= 100) s_pan = 100;
    else s_pan += 1;

    if (old_s_pan != s_pan) repaint = true;
  }
  if (event_map[KP_2_DOWN])
  {
    old_s_pan = s_pan;

    if (s_pan <= 1) s_pan = 1;
    else s_pan -= 1;

    if (old_s_pan != s_pan) repaint = true;
  }
  if (event_map[KP_6_DOWN])
  {
    old_tbw = total_bar_width;

    if (total_bar_width >= 22) total_bar_width = 22;
    else total_bar_width += 1;

    if (old_tbw != total_bar_width) repaint = true;
  }
  if (event_map[KP_4_DOWN])
  {
    old_tbw = total_bar_width;

    if (total_bar_width <= 1) total_bar_width = 1;
    else total_bar_width -= 1;

    if (old_tbw != total_bar_width) repaint = true;
  }

  if (event_map[MOUSE_SCROLL_UP])
  {
    v_pan += 45;
    repaint = true;
  }
  if (event_map[MOUSE_SCROLL_DOWN])
  {
    v_pan -= 45;
    repaint = true;
  }

  if (event_map[MOUSE_LEFT_DOWN])
  {
    mouse_x_old = mouse_x;
    mouse_y_old = mouse_y;
  }
}

void pe_ngraph(void)
{

}

void pe_symbols(void)
{

}

void pe_trades(void)
{

}

void pe_logic(void)
{

}

void pe_settings(void)
{

}




void clear_event_map(void)
{
  for (uint i = 0; i < END_OF_LIST; i++)
  {
    event_map[(Input_Type)i] = false;
  }
}


void mpe::Events::process_events(void)
{
  // Global events
  pe_global();

  switch (Active_Screen)
  {
    case LOG:
      pe_log();
      break;
    case GRAPH:
      pe_graph();
      break;
  	case NORMALIZED_GRAPH:
      pe_ngraph();
      break;
  	case SYMBOLS:
      pe_symbols();
      break;
  	case TRADES:
      pe_trades();
      break;
  	case LOGIC:
      pe_logic();
      break;
  	case SETTINGS:
      pe_settings();
      break;
  };

  clear_event_map();
}
