
# TODO

## Bugs

CSV: Quoted newlines do not work

## Wishlist

Unfiltered, Unprioritized, Un-triaged wishlist.

### Features

* `del` -- requires univalue update
* `get` inputs multiple paths, concat results together
* brackets [] used in json path for quoting special chars
* `get` array range (slice)
* pretty-printer:
	* tab output (versus space)
	* color output
	* sort keys
* core univalue
	* sort keys
	* delete
* append to array, possibly at root
* merge into object, possibly at root

### Efficiency

* mmap support for file input; eliminate double (and occasionally triple) buf
* future proof: regen tests output with --min

