# Contributing

tinyff is very open to patches from anyone around the world fixing anything. The following pages under
this section will talk about different rules, styles, and any other information that contributers will need. **And it is heavily
recommended that you read through this whole section, even if a section doesn't apply to you because it will most likely
without you even knowing** (and it also makes contributing in the future easier).

## Ground rules

**Warning the people skipping over this section: unless you've read this before, you should most likely read it. And if you
have already read it, it's better to go over it again because it is subject to change a lot in the earlier stages.**

tinyff has some strict rules for patches, so don't be surprised if your patch gets rejected quickly because of one
of these reasons:

### `master` contains only fixes and release-ready features
This is the most important rule throughout the whole contribution line. Once you submit a patch, be aware
it will most likely not be merged to master for a long while. This is not because your contribution is not
important, but rather because **humans make mistakes** so it is better for you to have your own branch for your feature
so that other reviewers and developers can test it before being ready for the premerge process.

### tinyff uses patch-driven development
PRs have been disabled on [GitHub](https://github.com/TazyFoundSoup/tinyff.git) for a reason. After thoughtful consideration,
patches are the only way to contribute to tinyff now. When you want to submit, read thoughtfully through the [patch submission guide](patch.md)
when you are ready to submit your patch.

### A patch is *not* a complete submission
When actually submitting your patch, please state *in details* about the changes in your patch. A short description is enough
if the code is readable. If the code is not very readable, please explain what it is in details. And, no, don't use buzzwords to
make yourself look high and mighty. If reviewers can't understand it, you should probably read through the full rule book about
what makes a good description. Alongside your description, you should have a clear tag about the target branch you are hoping for
you patch to merged into. It should be in it's own line in the Matrix message:

`Target: feat/<topic>`

or if you are submitting a small change able to be merged into master easily:

`Target: master`

This should be only for:
- non-binary changing changes like code style changes
- documentation changes, like typo fixes or markdown fancy schmancy styling
- small fixes to code which having a seperate branch would be overkill

Even after being put into a seperate branch, **it still does mean it is closer to being merged**, so don't submit and patch
and not expect a reviewer to ask questions about the patch. Even if it seems silly, we will not have a Jia Tan incident.

### No shotguns
Please, **PLEASE**, do not submit a patch changing features over multiple scopes, and god forgive you if they aren't even well
described (as stated in the previous rule). These will be immediately be rejected so **just don't**

These are the unbreakable rules and will probably be subject to lots of change in the future

## Getting involved

WIP
