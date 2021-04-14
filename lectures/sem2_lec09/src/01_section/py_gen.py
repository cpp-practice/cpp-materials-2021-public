
def my_gen(n):
    for i in range(n):
        yield 2*i


for i in my_gen(10):
    print(i)


