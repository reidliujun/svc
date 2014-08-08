function result = happyDegree(layer_list)

layerMultiple = 1;
layerSum = 0;

for idx=1:length(layer_list)
    layerMultiple = layerMultiple * layer_list(idx);
    if idx ~= length(layer_list)
        layerSum = layerSum + (layer_list(idx+1)-layer_list(idx))^2;
    end
end
result = layerMultiple^(1/length(layer_list))-(1/(length(layer_list)-1))*layerSum;


