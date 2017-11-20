
% Computes the gradient of the softplus activation function.

function f = softplus_grad(X)

f = diag(1./(1 + exp(-X)));

end
