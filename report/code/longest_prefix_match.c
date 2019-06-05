rt_entry_t *longest_prefix_match(u32 dst)
{
    rt_entry_t * result = NULL;
    int max_mask = 0;
    rt_entry_t * ele = NULL;
    list_for_each_entry(ele, &rtable, list) {
        if((ele->dest & ele->mask) == (dst & ele->mask)) {
            if(ele->mask >= max_mask) {
                max_mask = ele->mask;
                result = ele;
            }
        }
    }
    return result;
}