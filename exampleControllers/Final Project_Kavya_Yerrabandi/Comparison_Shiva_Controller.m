clear all;

%....Results taken from 15 simulations...%
MDP_Kavya = [121; 104; 116; 185; 118; 90; 93; 93; 123; 97; 128; 118; 139; 129; 132]; %... Time taken with my gradient model

BFS_SHIVA = [98; 105; 154; 244; 154; 123; 123; 116; 140; 211; 249; 425; 490; 263; 217];%...Time taken with Shiva's gradient model

%...to plot the boxplot...
Time = [MDP_Kavya; BFS_SHIVA];

group = ['MDP_Kavya';'MDP_Kavya';'MDP_Kavya';'MDP_Kavya';'MDP_Kavya';'MDP_Kavya';'MDP_Kavya';'MDP_Kavya';'MDP_Kavya';'MDP_Kavya';'MDP_Kavya';'MDP_Kavya';'MDP_Kavya';'MDP_Kavya';'MDP_Kavya';'BFS_SHIVA';'BFS_SHIVA';'BFS_SHIVA';'BFS_SHIVA';'BFS_SHIVA';'BFS_SHIVA';'BFS_SHIVA';'BFS_SHIVA';'BFS_SHIVA';'BFS_SHIVA';'BFS_SHIVA';'BFS_SHIVA';'BFS_SHIVA';'BFS_SHIVA';'BFS_SHIVA'; ];

boxplot(Time,group);
title('Time taken by the Robots for each gradient model');
xlabel('Gradient Model used');
ylabel('Time in Seconds');