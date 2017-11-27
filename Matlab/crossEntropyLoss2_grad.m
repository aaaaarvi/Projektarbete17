
% Computes the gradient of the alternate cross-entropy loss function.

function f = crossEntropyLoss2_grad(Yh, Y)

f = -Y./max(1e-20, Yh) + (1 - Y)./max(1e-20, 1 - Yh)/1000;

end
