img = imread('peppers.png');  %# Load a sample 3-D RGB image
img(10:10:end,:,:) = 0;  %# Change every tenth row to black

img(:,10:10:end,:) = 0;       %# Change every tenth column to black
imshow(img);                  %# Display the image

cam = webcam(2);

originalImage = snapshot(cam);
img = imcrop(originalImage,[345 60 1110 850]);
filename = 'testImage';
imwrite(img,strcat(filename,'.png'));

img(1:10,:,1) = 255;  % Change the red value for the first pixel
img(1:10,:,2) = 0;    % Change the green value for the first pixel
img(1:10,:,3) = 0;    % Change the blue value for the first pixel
% imwrite(im,'new.jpeg');  % Save modified image
% 
% 
% img(10:10:end,:,:) = 0;       %# Change every tenth row to black
% img(:,10:10:end,:) = 0;       %# Change every tenth column to black
imshow(img);                  %# Display the image
% make grayscale.
%originalImage = rgb2gray(originalImage);
%imshow(originalImage)% (*This command shows the image*)
%roipoly(img)

clear('cam'); % (*turns of the camera*)


img = imread('peppers.png');  % Load a jpeg image
img(1:10,:,1) = 255;  % Change the red value for the first pixel
img(1:10,:,2) = 0;    % Change the green value for the first pixel
img(1:10,:,3) = 0;    % Change the blue value for the first pixel
imwrite(img,'new.jpeg');  % Save modified image