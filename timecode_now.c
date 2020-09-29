// Conversation about how this is used with ffmpeg to record timecode
// https://twitter.com/PaulStoffregen/status/1310975743828975616

// gcc -O2 -Wall -o timecode_now timecode_now.c

// ffmpeg needs the drop frame format to create quicktime .mov which
// Final Cut can import.  Using the non-drop frame format creates files
// which Final Cut erronously believes are hours long.

// However, ffmpeg does an adjustment from the value given as input
// to the timecode number which ends up in the .mov metadata.  That
// adjustment isn't quite right for 59.94 Hz video.
//   https://stackoverflow.com/questions/48912476

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

int main()
{
	struct timeval tv;
	struct tm t;
	const int fps = 60;

	// get the actual time
	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &t);

	//for testing specific cases
	//t.tm_hour = 9;
	//t.tm_min = 20;
	//t.tm_sec = 21;
	//tv.tv_usec = 15 * 1000000 / fps;

	// convert actual to number of video frames at 60 Hz
	int total_frames = (t.tm_hour * 3600 + t.tm_min * 60 + t.tm_sec) * fps
		+ (int)tv.tv_usec * fps / 1000000;
	//printf("total frames = %d\n", total_frames);

	// adjust by the algorithm ffmpeg uses
	int tmin = t.tm_hour * 60 + t.tm_min;
	//total_frames -= ((fps == 30) ? 2 : 4) * (tmin - tmin/10); // newer ffmpeg?
	total_frames -= 2 * (tmin - tmin/10); // ffmpeg 3.4.8-0ubuntu0.2
	//printf("adjusted frames = %d\n", total_frames);

	// convert number of video frames back to time
	int frame = total_frames % fps;
	int total_sec = total_frames / fps;
	int total_min = total_sec / 60;
	int hours = total_min / 60;
	int minutes = total_min - hours * 60;
	int seconds = total_sec - (minutes * 60 + hours * 3600);

	//printf("%02d:%02d:%02d.%02d\n", t.tm_hour, t.tm_min, t.tm_sec, tv.tv_usec * fps / 1000000);
	printf("%02d:%02d:%02d.%02d\n", hours, minutes, seconds, frame);
	return 0;
}
