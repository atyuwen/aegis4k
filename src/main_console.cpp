#include "common.hpp"
#include "mzk.hpp"

int EntryPoint()
{	
	InitSound();

	const char logo[] = "           ___              _      __ __  __   \n"
						"          /   | ___  ____ _(_)____/ // / / /__ \n"
						"         / /| |/ _ \/ __ `/ / ___/ // /_/ //_/ \n"
						"        / ___ /  __/ /_/ / (__  )__  __/ ,<    \n"
						"       /_/  |_\___/\__, /_/____/  /_/ /_/|_|   \n"
						"                  /____/                     \n\n";

	const char info[] = "          composed via 4klang synthesizer\n";
	const char auth[] = "                 atyuwen@gmail.com\n\n";

	HANDLE h_std_out = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsole(h_std_out, logo, sizeof(logo)  - 1, NULL, NULL);
	WriteConsole(h_std_out, info, sizeof(info)  - 1, NULL, NULL);
	WriteConsole(h_std_out, auth, sizeof(auth)  - 1, NULL, NULL);

	char play_bar[] = "\r|--------------------------------------------------| [00:00/00:00]";

	int cur_sample = 0;
	while (cur_sample < MAX_SAMPLES && !GetAsyncKeyState(VK_ESCAPE))
	{
		int progroess = cur_sample * 50 / MAX_SAMPLES;
		play_bar[progroess + 2] = '>';
		if (progroess > 0) play_bar[progroess + 1] = '-';

		int total_time = MAX_SAMPLES / SAMPLE_RATE;
		int current_time = cur_sample / SAMPLE_RATE;

		play_bar[56] = current_time / 60 / 10 + '0';
		play_bar[56] = current_time / 60 % 10 + '0';
		play_bar[58] = current_time % 60 / 10 + '0';
		play_bar[59] = current_time % 60 % 10 + '0';

		play_bar[61] = total_time / 60 / 10 + '0';
		play_bar[62] = total_time / 60 % 10 + '0';
		play_bar[64] = total_time % 60 / 10 + '0';
		play_bar[65] = total_time % 60 % 10 + '0';

		WriteConsole(h_std_out, play_bar, sizeof(play_bar)  - 1, NULL, NULL);
		Sleep(10);
		cur_sample = GetCurrentSample();
	}

	WriteConsole(h_std_out, "\n", 1, NULL, NULL);
	ExitProcess(0);
	return 0;
}

#ifndef RELEASE
int main()
{
	return EntryPoint();
}
#endif
