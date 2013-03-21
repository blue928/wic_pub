
CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Installation
 * How to set up publications to appear in the form

INTRODUCTION
------------

Current maintainer: David Cameron
Original author: David Cameron <david.a.cameron@sbcglobal.net>

The Publication Order Form module was created for the WIC office at NAL.  It
creates a new node type named WIC Publication, a taxonomy vocabulary named
Publication order form terms, and a multi-step order form located at
/publication-order-form.  The form is generated using the ctools multi-step form
API.

When an order is submitted through the form two emails are sent: one to the
email address supplied by the user on the form and another to the WIC staff who
handles orders.

The module configuration page is located at admin/wic-forms/order-form.
One configuration option is available: the WIC staff email to which order
information emails will be sent.


INSTALLATION
------------

Install as usual, see http://drupal.org/node/895232 for further information.


HOW TO SET UP PUBLICATIONS TO APPEAR IN THE FORM
------------------------------------------------

The hierarchy of the Publication order form terms vocabulary is what determines
the structure of the multi-step order form.  Top-level terms in the vocabulary
are used to generate separate pages in the form.  Second-level terms are used
as sub-headings on their parent term's page.  Any lower-level terms will be
ignored and nodes tagged with those terms will not appear on the form.

To make an item appear on the form, create a WIC Publication node.  Then tag it
with a term from the order form terms vocabulary.  The node will automatically
appear in the form.

Term 1
   |   \
   |    Term 3
   |    |
   |    Term 4
   |
Term 2

In the sample taxonomy hierarchy shown above terms 1 and 2 would be used to
create two separate pages in the multi-step order form.  Nodes tagged with
either of those terms will appear in a table at the top of that page.

Terms 3 and 4 will appear as sub-headings on the term 1 page.  Nodes tagged with
one of those terms will appear in a table beneath the sub-heading.