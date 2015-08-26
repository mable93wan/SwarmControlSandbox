% ******************Loading input Image**********************************
cam = webcam(1); %Defines the webcam.
% Read in a webcam snapshot.
originalImage = snapshot(cam);
% Crop the image to use ROI.
originalImage = imcrop(originalImage,[864 230 525 370]);
% make grayscale.
%originalImage = rgb2gray(originalImage);

%originalImage = originalImage(33:33+550,250:250+810,:);  %ROI, you can use imrect to draw a rectangle using your mouse.
imshow(originalImage)
% Too easy, from http://www.mathworks.com/help/images/examples/detect-and-measure-circular-objects-in-an-image.html
tic
[centers, radii] = imfindcircles(originalImage,[16 26],'ObjectPolarity','dark','Sensitivity',0.97);
toc
M = mean(centers);
V = var(centers);
C = cov(centers);


imshow(originalImage);
%Draw the circles
h = viscircles(centers,radii);
hold on
plot(M(1,1) , M(1,2),'*','Markersize',16)
plot_gaussian_ellipsoid(M,C);
plot(centers(:,1),centers(:,2),'+','Markersize',16)
hold off
clear('cam');