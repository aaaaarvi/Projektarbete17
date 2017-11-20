
% Computes the gradient of the softmax activation function.

function f = softmax_grad(X)

%expsum = sum(exp(X));
%f = exp(X)/expsum - exp(2*X)/((expsum)^2);

smX = softmax(X);
f = -smX*smX';
f = f + diag(smX);

end
