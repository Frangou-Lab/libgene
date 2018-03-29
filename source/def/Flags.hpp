/*
 * Copyright 2018 Frangou Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LIBGENE_DEF_FLAGS_HPP_
#define LIBGENE_DEF_FLAGS_HPP_

class Flags {
 public:
    // *Checkbox* - "Search both strands"
    static const char kForwardAndReverseComplementsSearch[];

    // *Checkbox* – "Override quality"
    static const char kOverrideExistingQuality[];

    // *Checkbox* – "Omit quality"
    static const char kOmitQuality[];

    // *Checkbox* – "RNA input"
    static const char kRnaSequenceSearch[];

    // *Checkbox* – "RNA primers"
    static const char kRnaPrimersSearch[];

    // *Checkbox* – "Error correct"
    static const char kDemultiplexWithErrorCorrection[];

    // *Checkbox with corresponding field* – "Context length"
    static const char kContextEnabled[];

    // *Checkbox* - "Barcode in sequence"
    static const char kTagIsInSequence[];
    
    // *Checkbox with corresponding field* – "Hamming distance" (previously,
    // "Allow mismatches")
    static const char kMismatchesEnabled[];

    // *Pop Up Button* metric selector
    static const char kSelectedMetric[];
    
    //
    static const char kIlluminaR2Tags[];

    // *Checkbox with corresponding field* - "Columns order" (right side)
    static const char kReorderOutputColumns[];

    // *Checkbox with corresponding field* - "Columns order" (left side)
    static const char kReorderInputColumns[];

    // *Checkbox with corresponding field* "max" – "Limit amplicon length"
    static const char kMaxAmpliconSize[];

    // *Checkbox with corresponding field* "min" – "Limit amplicon length"
    static const char kMinAmpliconSize[];

    // *Checkbox with corresponding field* - "Barcode in sequence"
    static const char kSolexaFastqCutoffLength[];
    
    // *Radio-button* – "Coupled queries"
    static const char kCoupledQueries[];

    // *Radio-button* – "Paired query"
    static const char kPairedQueryExtraction[];

    // *Radio-button* – "Two-set search"
    static const char kMixedStrainPairedPrimerSearch[];

    // *Radio-button* – "Barcodes"
    static const char kDemultiplexByTags[];
    
    // *Radio-button* – "Binding targets"
    static const char kSearchBindingTargets[];

    // *Drop-down menu* - "Output"
    static const char kOutputFormat[];

    // *Drop-down menu* – "Input format"
    static const char kInputFormat[];

    // *Textfield* – "Override quality"
    static const char kFastqQuality[];

    // * no specific option * - selected if input has .gb or .gbk extension
    static const char kGenBankInputFormat[];

    // DEPRECATED - always on
    static const char kVerbose[];

    // DEPRECATED, kFastqQuality took over
    static const char kDefaultQuality[];

    // Various suffixes specific to different FASTQ formats
    static const char kIllumina1_8Suffix[];
    static const char kIllumina1_5Suffix[];
    static const char kIllumina1_3Suffix[];
    static const char kSangerSuffix[];
    static const char kSolexaSuffix[];
};

#endif  // LIBGENE_DEF_FLAGS_HPP_
