% img = imread('peppers.png');  %# Load a sample 3-D RGB image
% img(10:10:end,:,:) = 0;  %# Change every tenth row to black
% 
% img(:,10:10:end,:) = 0;       %# Change every tenth column to black
% imshow(img);                  %# Display the image
% 
cam = webcam(2);

originalImage = snapshot(cam);
img = imcrop(originalImage,[345 60 1110 850]);
% filename = 'testImage2';
% imwrite(img,strcat(filename,'.png'));

%img = imread('testImage2.png');  % Load a jpeg image
goalX = 10;
goalY = 10;
s = size(img);
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
                if img(i*10+k, j*10+l,1) > 250 && map(i,j) ~= 1 && img(i*10+k, j*10+l,2) < 160 && img(i*10+k, j*10+l,3)>190
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
Policy = MDPgridworldExample(map,goalX,goalY);
clear('cam'); % (*turns of the camera*)

% img = imread('testImage2.png');  % Load a jpeg image
% test = 550;
% test2 = 350;
% img(test,test2,:)
% img(test:test+10,test2:test2+10,1)= 255;
% img(test:test+10,test2:test2+10,2)= 0;
% img(test:test+10,test2:test2+10,3)= 0;
% imshow(img)
      
% img(10:10,1:10,1) = 255;
% img(1:10,1:10,2) = 0;
% img(1:10,1:10,3) = 0;
% imwrite(img,'new.jpeg');
% img(1:10,:,1) = 255;  % Change the red value for the first pixel
% img(1:10,:,2) = 0;    % Change the green value for the first pixel
% img(1:10,:,3) = 0;    % Change the blue value for the first pixel
% imwrite(img,'new.jpeg');  % Save modified image
