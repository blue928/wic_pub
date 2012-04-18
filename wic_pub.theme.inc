<?php
// $Id$

/**
 * @file
 * Contains the theme functions for the wic_pub module.
 *
 * For a full list of elements and attributes, see:
 * http://api.drupal.org/api/drupal/developer--topics--forms_api_reference.html/7
 */

/**
 * Returns HTML for the WIC publication full node view
 */
function theme_wic_pub_full_view($variables) {
	$node = $variables['node'];

	if (node_is_page($node)) {
		$breadcrumb = array();
		$breadcrumb[] = l(t('Home'), NULL);
		$breadcrumb[] = l(t('WIC Publications'), 'wic-publications');
		$breadcrumb[] = $node->title;
		drupal_set_breadcrumb($breadcrumb);
	}

  if ($node->wic_pub_order_number) {
	  $node->content['wic_pub_order_number'] = array(
      '#markup' => theme('wic_pub_order_number', array('order_number' => $node->wic_pub_order_number)),
			'#weight' => 0,
		);
	}
  if ($node->wic_pub_description) {
	  $node->content['wic_pub_description'] = array(
      '#markup' => theme('wic_pub_description', array(
			  'description' => $node->wic_pub_description,
			  'in_stock' => $node->wic_pub_in_stock,
				'quantity_max' => $node->wic_pub_quantity_max,
			)),
			'#weight' => 1,
		);
	}
}

/**
 * Returns HTML for the WIC publication teaser node view
 */
function theme_wic_pub_teaser_view($variables) {
	$node = $variables['node'];

  if ($node->wic_pub_description) {
	  $node->content['wic_pub_description'] = array(
      '#markup' => theme('wic_pub_description', array('description' => $node->wic_pub_description)),
			'#weight' => 0,
		);
	}
}

/**
 * Returns HTML for the Order number field
 */
function theme_wic_pub_order_number($variables) {
	$order_number = $variables['order_number'];
	return "<div class='wic-pub-field wic-pub-order-number'>
	  <h3 class='field-label'>" . t('Order number') . ":</h3>
		<p class='field-item'>$order_number</p>
		</div>\n";
}

/**
 * Returns HTML for the Description field
 */
function theme_wic_pub_description($variables) {
	$description = $variables['description'];
	$in_stock = $variables['in_stock'];
	$quantity_max = $variables['quantity_max'];
	$output = "<div class='wic-pub-field wic-pub-description'>
	  <h3 class='field-label'>" . t('Description') . ":</h3>\n";
	$output .= "<p class='field-item'>";
	if (!$in_stock) {
	  $output .= "<span class='line-through'>$description</span> <em>currently out of stock</em>";
	}
	elseif ($quantity_max) {
		$output .= "$description <strong>(Limit of $quantity_max)</strong>";
	}
	else {
		$output .= $description;
	}
	$output .=  "</p>
		</div>\n";
	return $output;
}

function theme_wic_pub_order_form_description($variables) {
	$description = $variables['description'];
	$in_stock = $variables['in_stock'];
	$quantity_max = $variables['quantity_max'];
	if (!$in_stock) {
	  $output = "<span class='line-through'>$description</span> <em>currently out of stock</em>";
	}
	elseif ($quantity_max) {
		$output = "$description <strong>(Limit of $quantity_max)</strong>";
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