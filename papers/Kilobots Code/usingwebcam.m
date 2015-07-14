% webcamlist % (*Show the list of webcams*)
cam = webcam(1);

img1 = snapshot(cam);

img2 = snapshot(cam);
img3 = snapshot(cam);
img4 = snapshot(cam);
img5 = snapshot(cam);



 imshow(img1)% (*This command shows the image*)
 figure
 imshow(img2)% (*This command shows the image*)
 figure
 imshow(img3)% (*This command shows the image*)
 figure
 imshow(img4)% (*This command shows the image*)
 figure
 imshow(img5)% (*This command shows the image*)
%roipoly(img)

%clear('cam'); % (*turns of the camera*)