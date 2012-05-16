<?php
// $Id$

/**
 * @file
 * Contains the theme functions for the wic_pub module.
 *
 * For a full list of elements and attributes, see:
 * http://api.drupal.org/api/drupal/developer--topics--forms_api_reference.html/7
 */

function theme_wic_pub_order_form_description($variables) {
	$description = $variables['description'];
	$in_stock = $variables['in_stock'];
	$max_quantity = $variables['max_quantity'];
	if (!$in_stock) {
	  $output = "<span class='line-through'>$description</span> <em>currently out of stock</em>";
	}
	elseif ($max_quantity) {
		$output = "$description <strong>(Limit of $max_quantity)</strong>";
	}
	else {
		$output = $description;
	}
	return $output;
}

function theme_wic_pub_order_form($variables) {
	$form = $variables['form'];
	// Return if we're on the last page.  The clinic info does not get themed.
	if (isset($form['name'])) {
		return;
	}

  $header = array(t('Quantity'), t('Order number'), t('Description'));
	$output = '';
  foreach (element_children($form['items']) as $group) {
		if ($group != 'no_heading') {
			$output .= '<h3>' . t($group) . "</h3>\n";
		}
		$rows = array();
		foreach (element_children($form['items'][$group]) as $nid) {
			$row = array();
			$row['data'][] = drupal_render($form['items'][$group][$nid]['quantity']);
			$row['data'][] = drupal_render($form['items'][$group][$nid]['printed_order_number']);
			$row['data'][] = drupal_render($form['items'][$group][$nid]['printed_description']);
			$rows[] = $row;
		}

    $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'wic-pub-order-form-table')));
	}
	$output .= drupal_render_children($form);
	return $output;
}