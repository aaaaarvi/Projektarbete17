
% Computes the gradient of the rectifier activation function.

function f = relu_grad(X)

f = diag(1.*(X > 0));

end
