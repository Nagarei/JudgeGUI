#pragma once

inline dxle::sizei My_GetWindowSize() {
	dxle::sizei window_size;
	DxLib::GetWindowSize(&window_size.width, &window_size.height);//@todo dxlibex
	if (window_size == 0) {
		window_size.width = 320, window_size.height = 240;
	}
	return window_size;
}