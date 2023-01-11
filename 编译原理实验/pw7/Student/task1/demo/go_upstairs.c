int num[2] = {4, 8};
int x[1];
int n;
int tmp = 1;

int climbStairs(int n) {
    if(n < 4)
        return n;
    int dp[10];
    dp[0] = 0;
    dp[1] = 1;
    dp[2] = 2;
    int i;
    i = 3;
    while(i<n+1){
        dp[i] = dp[i-1] + dp[i-2];
        i = i + 1;
    }
    return dp[n];
}

int main(){
    int res;
    n=num[0];
    x[0] = num[tmp];
    res = climbStairs(n + tmp);
    return res - x[0];
}