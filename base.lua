--print('******************base.lua begin ******************') 
function printval(val,c)
    print("get vail is :",val,c.inadd(10,30))
end

function addordec(lef,right,op)
    if 0 == op then
        return lef+right
    elseif 1 == op then
        return lef - right
    else
        return lef*right
    end
end
--print('******************base.lua end******************') 