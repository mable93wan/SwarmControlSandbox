function goalX = my_callback_fcn(goalX, goal1x, goal2x)
    if goalX == goal1x
        goalX = goal2x;
    else
        goalX = goal1x;
    end
end