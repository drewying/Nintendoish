#pragma once
namespace NES {
	class Controller
	{
	public:
		
		unsigned char pollController();
		void startPoll();

		bool a      = false;
		bool b      = false;
		bool select = false;
		bool start  = false;
		bool up     = false;
		bool down   = false;
		bool left   = false;
		bool right  = false;

		Controller();
		~Controller();
	
	private:
		bool strobeLatch = false;
		int currentPoll = 0;
	};


}


