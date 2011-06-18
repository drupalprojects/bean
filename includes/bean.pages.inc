<?php

/**
 * @file
 * Bean Functions
 */

/**
 * View the Bean
 */
function bean_view($bean) {
  drupal_set_title($bean->title);
  return $bean->view();
}

/**
 * Edit bean page callback
 */
function bean_edit($bean) {
  return drupal_get_form('bean_form', $bean);
}

/**
 * Add bean page callback
 */
function bean_add($type) {
  $bean = bean_create(array('type' => $type));

  return drupal_get_form('bean_form', $bean);
}

/**
 * Menu Callback to list available bean types to add
 */
function bean_add_page() {
  $bean_types = bean_get_types();

  // Bypass the block/add listing if only one block type is available.
  if (count($bean_types) == 1) {
    $bean_type = array_shift($bean_types);
    drupal_goto('block/add/' . $bean_type->buildURL());
  }

  return theme('bean_add_list', array('content' => $bean_types));
}

function bean_list() {
  $rows = array();
  
  // Build the sortable table header.
  $header = array(
    'title' => array('data' => t('Title'), 'field' => 'b.label', 'sort'=>'asc'),
    'type' => array('data' => t('Type'), 'field' => 'b.type'),
    'operations' => array('data' => t('Operations')),
  );
  
  $sql_query = db_select('bean', 'b');
  
  $sql_query->fields('b', array('bid', 'label', 'type'));
  $sql_query = $sql_query->extend('TableSort')->extend('PagerDefault')->limit(50);
  $beans = $sql_query->orderByHeader($header)->execute();
  
  $destination = drupal_get_destination();
  
  foreach($beans as $bean) {
    $rows[$bean->bid] = array (
    'title' => array(
      'data' => array(
        '#type' => 'link',
        '#title' => $bean->label,
        '#href' => 'block/' . $bean->bid,
      ),
    ),
    'type' => check_plain($bean->type),
    );

    // Build a list of all the accessible operations for the current bean.
    $operations = array();
    $operations['edit'] = array(
      'title' => t('edit'),
      'href' => 'block/' . $bean->bid . '/edit',
      'query' => $destination,
    );
    $operations['delete'] = array(
      'title' => t('delete'),
      'href' => 'block/' . $bean->bid . '/delete',
      'query' => $destination,
    );
    // Render an unordered list of operations links.
    $rows[$bean->bid]['operations'] = array(
      'data' => array(
        '#theme' => 'links__node_operations',
        '#links' => $operations,
        '#attributes' => array('class' => array('links', 'inline')),
      ),
    );
  }

  $output = theme('table', array('header' => $header, 'rows' => $rows));
  $output .= theme('pager');
  
  return $output;
}

/**
 * Returns HTML for a list of available bean types for bean creation.
 *
 * @param $variables
 *   An associative array containing:
 *   - content: An array of bean types.
 *
 * @ingroup themeable
 */
function theme_bean_add_list($variables) {
  $content = $variables['content'];
  $output = '';

  if ($content) {
    $output = '<dl class="bean-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l('Add ' . $item->getLabel(), 'block/add/' . str_replace('_', '-', $item->type)) . '</dt>';
    }
    $output .= '</dl>';
  }
  else {
    //$output = '<p>' . t('You have not created any block types yet. Go to the <a href="@create-content">block type creation page</a> to add a new bean type.', array('@create-content' => url('admin/structure/block/types/add'))) . '</p>';
    $output = '<p>' . t('You have not created any block types yet.') . '</p>';
  }
  return $output;
}

/**
 * Bean form
 */
function bean_form($form, &$form_state, $bean) {
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#required' => TRUE,
    '#default_value' => $bean->label,
    '#description' => t('Name that displays in the admin interface'),
  );

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('The Title of the block.'),
    '#default_value' => $bean->title,
  );

  $form['bean'] = array(
    '#type' => 'value',
    '#value' => $bean,
  );

  field_attach_form('bean', $bean, $form, $form_state);

  // Get the Bean type form
  $form += $bean->getForm();

  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 200,
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Validation for bean form
 */
function bean_form_validate($form, &$form_state) {
  $bean = $form_state['values']['bean'];
  $bean->label = $form_state['values']['label'];
  $bean->title = $form_state['values']['title'];

  field_attach_form_validate('bean', $bean, $form, $form_state);
  $form_state['values']['bean'] = $bean;

  $bean->validate($form_state['values']);
}

/**
 * Submit function for bean form
 */
function bean_form_submit($form, &$form_state) {
  $bean = $form_state['values']['bean'];
  $bean->setValues($form_state['values']);

  field_attach_submit('bean', $bean, $form, $form_state);
  $bean->save();
  $form_state['redirect'] = $bean->viewURL();;
}

/**
 * Menu callback -- ask for confirmation of bean deletion
 */
function bean_delete_confirm($form, &$form_state, $bean) {
  $form['#bean'] = $bean;

  return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $bean->label)),
    'block/' . $bean->bid,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Execute node deletion
 */
function bean_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $bean = $form['#bean'];
    $bean->delete();
    watchdog('block', '@type: deleted %title.', array('@type' => $bean->type, '%title' => $bean->label));
    drupal_set_message(t('@type %title has been deleted.', array('@type' => $bean->type, '%title' => $bean->label)));
  }

  $form_state['redirect'] = '<front>';
}