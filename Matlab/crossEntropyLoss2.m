
% Computes the value of the alternate cross-entropy loss function.

function f = crossEntropyLoss2(Yh, Y)

f = -Y'*log(max(1e-100, Yh)) - (1 - Y')*log(max(1e-100, 1 - Yh))/5000;

end
