/* A rexx test script */
mem = allocmem(50)
say c2x(mem)
say c2x(typepkt(mem))
say typepkt(mem,a)
say typepkt(mem,c)
say typepkt(mem,f)
mem2 = offset(mem, 32)
say c2x(mem2)
say c2x(import(mem2,4))
freemem(mem,50)
say test
