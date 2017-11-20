
% Computes the value of the softplus activation function.

function f = softplus(X)

f = zeros(length(X), 1);
for i = 1:length(X)
    if X(i) >= 50
        f(i) = X(i);
    else
        f(i) = log(1 + exp(X(i)));
    end
end

end
