
% Computes the value of the softmax activation function.

function f = softmax(X)

%expsum = sum(exp(X));
%f = exp(X)/expsum;

Xmax = max(X);
expsum = sum(exp(X - Xmax));
f = exp(X - Xmax)/expsum;

end
