/*
"ability"            "learning"          "grouping"
"basal"              "evaluative"        "modification"
"behavioral"         "objective"         "accountability"
"child-centered"     "congnitive"        "process"
"differentiated"     "enrichment"        "core"curriculum
"discovery"          "scheduling"        "algorithm"
"flexible"           "humanistic"        "performance"
"heterogeneous"      "integrated"        "reinforcement"
"homogeneous"        "non-graded"        "open classroom"
"manipulative"       "training"          "resource"
"modular"            "vertical"age       "structure"
"tavistock"          "monivational"      "facility"
"individualized"     "creative"          "enviroment"
*/
Do Line = 2 to 14
   L = Sourceline(Line)
   Parse Var l '"' A '"' . '"' B '"' . '"' C '"'
   L1 = Line - 1
   A.L1 = A   ; B.L1 = B ; C.L1 = C
End
 
A = Random(,,time('s'))
 
Do 10
   A = Random(1,13) ; B = Random(1,13) ; C = Random(1,13)
   Say A.A  B.B   C.C
End
