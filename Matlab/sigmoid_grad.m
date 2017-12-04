
% Computes the value of the sigmoid activation function.

function f = sigmoid_grad(X)

f = zeros(length(X), 1);
for i = 1:length(X)
    if X(i) > -50
        f(i) = exp(-X(i))/((1 + exp(-X(i)))^2);
    end
end

f = diag(f);

end
