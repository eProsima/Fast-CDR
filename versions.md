# v2.0.0

* Support of Extended CDR Representation: encoding versions 1 and 2.

    Introduced API break with previous versions.
    Mainly `CdrType` enumeration, used in the constructor of `Cdr,` was renamed to `CdrVersion`.
    And `DDSCdrPlFlag` enumeration was also renamed to `EncodingAlgorithmFlag`.
