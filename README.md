# DIY-ECG
Wearable  device made of an ESP32 microcontroller and  pulse sensor. To measures and records heart beat to be sent to an IoT. 
I want to have a working EGC that not only records the user's BPM but will dispaly it to an IoT. This case I picked ThingSpeak as my IoT since it uses MATLAB. 
I can use the MATLAB to display the heart wave form and BPM in ThingSpeak. I have most the code working for the ESP32. 
TWO problems are present that are stopping me from completeing my DIY. 
1) Is that after the code runs sucessfully and it loops at the end. The sensor gets a reading of ZERO the second time the code excutes in the loop. Why is it not getting a correct reading like the first time the code is ran. 
2) I have a waveform displaying in ThingSpeak but it is nothing like the waveform I get in Arduino inside the Serial Monitor. 
