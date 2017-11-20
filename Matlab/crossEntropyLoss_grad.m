
% Computes the gradient of the cross-entropy loss function.

function f = crossEntropyLoss_grad(Yh, Y)

f = -Y./max(1e-20, Yh);

end
