// ../buffer.o ../utf8.o
#include "../buffer.h"
#include "assert.h"

int main(void) {
    {
        buffer_t buf = buf_new(5);

        assertEq("Capacity is initialized to given", buf.capacity, 5);
        assertEq("Usage is initialized to 0", buf.used, 0);
        assert("Data is initialized", buf.data != NULL);

        buf_free(&buf);
    }

    {
        buffer_t buf = buf_new(1);
        char exp[] = "onetwo0three456five";
        char str[] = "two";

        buf_push_str(&buf, "one");
        buf_push(&buf, str, strlen(str));
        buf_push_int(&buf, 0);
        buf_push_str(&buf, "three");
        buf_push_int(&buf, 456);
        buf_push_str(&buf, "five");

        assertEqBuf("Buffer contains right data", buf.data, buf.used, exp, strlen(exp));
        assert("Capacity is valid", buf.capacity >= buf.used);

        buf_free(&buf);
    }

    {
        buffer_t buf = buf_new(1);
        wchar_t codes[] = {0x24b62, 'a', 0x00a2};
        char exp[] = {0xf0, 0xa4, 0xad, 0xa2, 'a', 0xc2, 0xa2};

        buf_push_utf8(&buf, codes, 3);

        assertEqBuf("Buffer contains right data", buf.data, buf.used, exp, 7);

        buf_free(&buf);
    }

    return 0;
}
