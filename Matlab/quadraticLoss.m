
% Computes the value of the quadratic loss function.

function f = quadraticLoss(Yh, Y)

f = 0.5*sum((Yh - Y).^2);

end
