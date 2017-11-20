
% Computes the value of the cross-entropy loss function.

function f = crossEntropyLoss(Yh, Y)

f = -Y'*log(max(1e-100, Yh));

end
