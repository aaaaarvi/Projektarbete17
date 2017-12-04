
% Computes the value of the sigmoid activation function.

function f = sigmoid(X)

f = zeros(length(X), 1);
for i = 1:length(X)
    if X(i) > -50
        f(i) = 1/(1 + exp(-X(i)));
    end
end

end
