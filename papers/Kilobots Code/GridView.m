%%%%%%%%% By Shiva Shahrokhi Dec 2015: This code takes a snapshot of the
%%%%%%%%% webcam, then process it to find obstacles, gives that map to MDP
%%%%%%%%% and gets the result.


% img = imread('peppers.png');  %# Load a sample 3-D RGB image
% img(10:10:end,:,:) = 0;  %# Change every tenth row to black
% 
% img(:,10:10:end,:) = 0;       %# Change every tenth column to black
% imshow(img);                  %# Display the image
% 
% cam = webcam(2);
% % 
%  originalImage = snapshot(cam);
%  img = imcrop(originalImage,[345 60 1110 850]);
% filename = 'testImage15';
% imwrite(img,strcat(filename,'.png'));

img = imread('testImage12.png');  % Load a jpeg image
% Convert RGB image to chosen color space
I = rgb2hsv(img);

% Define thresholds for channel 1 based on histogram settings
channel1Min = 0.847;
channel1Max = 0.946;

% Define thresholds for channel 2 based on histogram settings
channel2Min = 0.174;
channel2Max = 1.000;

% Define thresholds for channel 3 based on histogram settings
channel3Min = 0.657;
channel3Max = 1.000;

% Create mask based on chosen histogram thresholds
BW = (I(:,:,1) >= channel1Min ) & (I(:,:,1) <= channel1Max) & ...
    (I(:,:,2) >= channel2Min ) & (I(:,:,2) <= channel2Max) & ...
    (I(:,:,3) >= channel3Min ) & (I(:,:,3) <= channel3Max);

goalX = 10;
goalY = 10;
s = size(BW);
sizeOfMap = floor(s/10);
map = zeros(sizeOfMap(1),sizeOfMap(2));
map(1,:) = 1;
map(:,1) = 1;
map(sizeOfMap(1),:) = 1;
map(:,sizeOfMap(2)) = 1;
for i= 1:sizeOfMap(1)-1
    for j = 1:sizeOfMap(2)-1
        for k = 0:9
            for l = 0:9
                if BW(i*10+k, j*10+l,1) > 0
                    map(i,j) = 1;
                    img(i*10:i*10+10,j*10:j*10+10,1) = 0;  % Change the red value for the first pixel
                    img(i*10:i*10+10,j*10:j*10+10,2) = 0;    % Change the green value for the first pixel
                    img(i*10:i*10+10,j*10:j*10+10,3) = 255;    % Change the blue value for the first pixel
                end
            end
        end
    end
end
imwrite(img,'new.jpeg');
[probability, movesX, movesY] = MDPgridworldExampleBADWALLS(map,goalX,goalY);
figure
imshow(img)
[X,Y] = meshgrid(1:size(map,2),1:size(map,1));
 hold on; hq=quiver(X*10,Y*10,movesY,movesX,0.5,'color',[0,0,0]); hold off
set(hq,'linewidth',2);
%clear('cam'); % (*turns of the camera*)

% % img = imread('testImage2.png');  % Load a jpeg image
% test = 550;
% test2 = 400;
% hold on
% img(test,test2,:)
% img(test:test+10,test2:test2+10,1)= 255;
% img(test:test+10,test2:test2+10,2)= 0;
% img(test:test+10,test2:test2+10,3)= 0;
% % imshow(img)
%  clear('cam');  
% img(10:10,1:10,1) = 255;
% img(1:10,1:10,2) = 0;
% img(1:10,1:10,3) = 0;
% imwrite(img,'new.jpeg');
% img(1:10,:,1) = 255;  % Change the red value for the first pixel
% img(1:10,:,2) = 0;    % Change the green value for the first pixel
% img(1:10,:,3) = 0;    % Change the blue value for the first pixel
% imwrite(img,'new.jpeg');  % Save modified image
