(ns assembler-cs-2200.core
  (:require [clojure.java.io :as io]
            [clojure.string :as str])
  (:gen-class))

(def inst-type
  {"add" "R-Type",
   "addi" "I-Type",
   "mul"  "R-Type",
   "sll"  "I-Type",
   "srl"  "I-Type",
   "and"  "R-Type",
   "or"   "R-Type",
   "lw"   "I-Type",
   "sw"   "I-Type",
   "j"    "J-Type",
   "jr"   "I-Type",
   "jal"  "J-Type",
   "beq"  "I-Type"})

(def opcodes
  {"add"  "000001",
   "addi" "000010",
   "mul"  "000011",
   "sll"  "000100",
   "srl"  "000101",
   "and"  "000110",
   "or"   "000111",
   "lw"   "001000",
   "sw"   "001001",
   "j"    "001010",
   "jr"   "001011",
   "jal"  "001100",
   "beq"  "001101"})

(def registers
  {"$0"    "00000",
   "$zero" "00000",
   "$at"   "00001",
   "$1"    "00001",
   "$v0"   "00010",
   "$2"    "00010",
   "$v1"   "00011",
   "$3"    "00011",
   "$a0"   "00100",
   "$4"    "00100",
   "$a1"   "00101",
   "$5"    "00101",
   "$a2"   "00110",
   "$6"    "00110",
   "$a3"   "00111",
   "$7"    "00111",
   "$t0"   "01000",
   "$8"    "01000",
   "$t1"   "01001",
   "$9"    "01001",
   "$t2"   "01010",
   "$10"   "01010",
   "$t3"   "01011",
   "$11"   "01011",
   "$t4"   "01100",
   "$12"   "01100",
   "$t5"   "01101",
   "$13"   "01101",
   "$t6"   "01110",
   "$14"   "01110",
   "$t7"   "01111",
   "$15"   "01111",
   "$s0"    "10000",
   "$16"   "10000",
   "$s1"   "10001",
   "$17"   "10001",
   "$s2"    "10010",
   "$18"   "10010",
   "$s3"   "10011",
   "$19"   "10011",
   "$s4"    "10100",
   "$20"   "10100",
   "$s5"   "10101",
   "$21"   "10101",
   "$s6"   "10110",
   "$22"   "10110",
   "$s7"   "10111",
   "$23"   "10111",
   "$t8"   "11000",
   "$24"   "11000",
   "$t9"    "11001",
   "$25"   "11001",
   "$k0"   "11010",
   "$26"   "11010",
   "$k1"   "11011",
   "$27"   "11011",
   "$gp"   "11100",
   "$28"   "11100",
   "$sp"   "11101",
   "$29"   "11101",
   "$fp"   "11110",
   "$30"   "11110",
   "$ra"   "11111",
   "$31"   "11111"})


(defn int-to-hex
  "parses an integer into a 16 bit (4 digit) hex string"
  [int-in]
  (let [parsed-int (Integer/parseInt int-in)] 
    (if (< parsed-int 0)
      (apply str (drop 4 (format "%04x" parsed-int)))
      (format "%04x" parsed-int))))

(defn bin-str-to-hex
  "parses a binary string into a hex string"
  [bin-str-in]
  (Integer/toString 
   (Integer/parseInt bin-str-in 2) 16))

(defn process-r-type
  "Processes an R-Type instruction"
  [instruction-map file-out]
  (let [opcode (get opcodes (get instruction-map :op))
        Rd (get registers (get instruction-map :arg1))
        Rb (get registers (get instruction-map :arg2))
        Rc (get registers(get instruction-map :arg3))]
    (do
     (println 
      (str  (get instruction-map :op) " " 
            (get instruction-map :arg1) " " 
            (get instruction-map :arg2) " " 
            (get instruction-map :arg3)))

     (println (str "Opcode: " opcode 
                   " Rb " Rb 
                   " Rc " Rc 
                   " Rd " Rd))

     (println 
      (bin-str-to-hex (str opcode Rb Rc Rd "00000000000")))

     (spit file-out 
           (bin-str-to-hex (str opcode Rb Rc Rd "00000000000"))
           :append true))))

(defn process-i-type
  "Processes an I-Type instructin"
  [instruction-map file-out]
  (let [opcode (get opcodes (get instruction-map :op))
        Rd (get registers (get instruction-map :arg1))
        Rb (get registers (get instruction-map :arg2))
        Imm (get instruction-map :arg3)]
    
    (do
      (println 
       (str  (get instruction-map :op) " " 
             (get instruction-map :arg1) " " 
             (get instruction-map :arg2) " " 
             (get instruction-map :arg3)))

      (println (str "Opcode: " opcode
                    " Rb: " Rb 
                    " Rd: " Rd
                    " Imm: " Imm))

      (println (str (bin-str-to-hex 
                     (str opcode Rb Rd))
                    (int-to-hex Imm)))

      (spit file-out 
            (str (bin-str-to-hex opcode)
                 (bin-str-to-hex Rb)
                 (bin-str-to-hex Rd)
                 (int-to-hex Imm))
            :append true))))

(defn process-j-type
  "Processes an J-Type instructin"
  [instruction-map file-out]
  (spit file-out
        (str (get instruction-map :op) " is a J-Type instruction\n")
        :append true))

(defn -main
  "This is an assembler for cs-2200 project) 1"
  [file-in file-out]
  (with-open [rdr (io/reader file-in)]
         (doseq [line (line-seq rdr)]
           (let [inst-map 
                 (zipmap [:op :arg1 :arg2 :arg3] 
                         (map str/trim (str/split line #"[,;]")))]
             (if-let [instruction-type (get inst-type (get inst-map :op))]
               (do
                 (if (= instruction-type "R-Type")
                   (process-r-type inst-map file-out)
                   (if (= instruction-type "I-Type")
                     (process-i-type inst-map file-out)
                     (if (= instruction-type "J-Type")
                       (process-j-type inst-map file-out))))))))))

