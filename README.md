# Cobra Hash

Cobra Hash is a custom hashing algorithm created by **JRDP Team**. It is designed to generate file hashes in a secure and unique way.

## Features

- Generates file hashes in the format `<BEGIN_COBRA_HASH><HASH><END_COBRA_HASH>`.
- Allows comparing the generated hash with a user-provided hash.
- Efficient data processing using 64-byte blocks.

## Usage

To use Cobra Hash:

# 1. Compile the program:

    gcc -o cobra_hash cobra_hash.c

   Generate a file hash:

    ./cobra_hash <filename>

  Verify the hash:

    ./cobra_hash -c <hash> <filename>

# Trademark

Cobra Hash and all related materials are the property of JRDP Team.
