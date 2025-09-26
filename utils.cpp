#define global_variable static
typedef unsigned int u32;

inline int clamp(int mini, int val, int maxi) {
    if (val < mini) return mini;
    return (val > maxi) ? maxi : val;
}