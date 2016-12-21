#ifndef KEY_INPUT_DATA_H_20150321_0919_
#define KEY_INPUT_DATA_H_20150321_0919_

class KeyInputData final {
	public:
		enum DirectionKey:uint8_t{KEY_UP,KEY_DOWN,KEY_LEFT,KEY_RIGHT,end_DirectionKey};
	private:
		uint_fast8_t GetKeyMask(DirectionKey direction);

		int last_arrow_key[end_DirectionKey];
		int now;
		char key_buf[256 * 2];
		char* now_key;
		char* last_key;
		KeyInputData();

		KeyInputData(const KeyInputData&) = delete;
		KeyInputData& operator=(const KeyInputData&) = delete;
	public:
		static inline KeyInputData& GetIns()
		{
			static KeyInputData Obj;
			return Obj;
		}
		void Update();
		//Mask : KEY_INPUT_...
		bool GetKeyInput(uint_fast8_t Mask);
		//Mask : KEY_INPUT_...
		bool GetNewKeyInput(uint_fast8_t Mask);
		bool GetDirectionKeyInput(DirectionKey direction);
};
#endif
