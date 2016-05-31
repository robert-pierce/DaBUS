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

(defn process-r-type
  "Processes an R-Type instructin"
  [instruction-map file-out]
  (spit file-out 
        (str (get instruction-map :op) " is an R-Type instruction\n")
        :append true))

(defn process-i-type
  "Processes an I-Type instructin"
  [instruction-map file-out]
  (spit file-out
        (str (get instruction-map :op) " is an I-Type instruction\n")
        :append true))

(defn process-j-type
  "Processes an J-Type instructin"
  [instruction-map file-out]
  (spit file-out
        (str (get instruction-map :op) " is a J-Type instruction\n")
        :append true))

(defn -main
  "This is an assembler for cs-2200 project 1"
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
                       (process-j-type inst-map file-out))))))
             ))))

