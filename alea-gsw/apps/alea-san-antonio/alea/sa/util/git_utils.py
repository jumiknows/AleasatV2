# File for python git utilities in SA

import subprocess

# hash functions
# WARNING: get_repo_hash will get hash only from HEAD of main repo, usually aleasat-software
def get_repo_hash():
    # Get FW hash
    result = subprocess.run(['git', 'rev-parse', 'HEAD'], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    # Remove whitespace
    FWgitHash = result.stdout.strip()
    # We only need first 8 characters (short hash)
    FWgitHashshort = FWgitHash[:8]

    return FWgitHashshort