import numpy as np
from math import e
class LogisticRegression:

    def __init__(self, penalty="l2", gamma=0.0):
        """gamma: constant term for penalty. larger values specify stronger regularization."""
        err_msg = "penalty must be 'l1' or 'l2', but got: {}".format(penalty)
        assert penalty in ["l2", "l1", ""], err_msg
        self.theta = None
        if penalty == 'l1':
            self.penalty_cost = lambda theta: gamma * np.sum(np.absolute(theta))
            self.penalty_learning = lambda theta: gamma * np.sign(theta)
        elif penalty == 'l2':
            self.penalty_cost = lambda theta: gamma / 2 * (theta * theta.T).item()
            self.penalty_learning = lambda theta: gamma * theta
        else:
            self.penalty_cost = lambda theta: 0
            self.penalty_learning = lambda theta: 0

    def sigmoid(self, x):
        """The logistic sigmoid function"""
        return 1 / (1 + np.power(e, -x))

    def fit(self, X, y, lr=0.01, tol=1e-7, max_iter=1e7):
        """
        Fit the regression coefficients via gradient descent or other methods 
        """
        m, n = X.shape
        theta = np.matrix(np.array([0.0] * n))
        costs = []
        
        for _ in range(int(max_iter)):
            learned = lr * ((self.sigmoid(X * theta.T) - y).T * X / m + self.penalty_learning(theta))
            if np.max(np.absolute(learned)) < tol:
                break
            theta -= learned

            y_pred = self.sigmoid(X * theta.T)
            cost_value = -np.sum(y.T * np.log(y_pred) + (1 - y).T * np.log(1 - y_pred)) / m + self.penalty_cost(theta)
            costs.append(cost_value)

        self.theta = theta
        return theta, costs
        
    def predict(self, X):
        """
        Use the trained model to generate prediction probabilities on a new
        collection of data points.
        """
        return self.sigmoid(X * self.theta.T)
