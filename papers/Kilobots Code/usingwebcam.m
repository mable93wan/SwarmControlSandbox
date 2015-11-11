% webcamlist % (*Show the list of webcams*)

cam = webcam(2);

originalImage = snapshot(cam);
originalImage = imcrop(originalImage,[345 60 1110 850]);
% make grayscale.
%originalImage = rgb2gray(originalImage);
imshow(originalImage)% (*This command shows the image*)
%roipoly(img)

clear('cam'); % (*turns of the camera*)